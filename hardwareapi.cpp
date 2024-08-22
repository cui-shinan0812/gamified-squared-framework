/*
 * This file contains the implementation of the Hardwaredriver class, which provides various hardware-related functionalities.
 * The class includes methods for initializing hardware, displaying frames, sending broadcast messages, and receiving data.
 */

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <random>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <iomanip>
#include <vector>
#include "hardwareapi.h"
using namespace std;

/*
int max_rows = 5; // N
int max_cols = 8; // M
*/

// Initialize the hardware driver with the given parameters
Hardwaredriver::Hardwaredriver(int controller_used, int rows, int cols, int *breakpoints, int num_breakpoints, string targetIP, int targetPort,
                                int localPort, int bufferSize)
{
    num_of_controller_used = controller_used; // number of controller used
    max_rows = rows; // Maximum number of pixel that a port can have among all controllers
    max_cols = cols; // Number of ports that are used
    breakpoints_length = new int[num_breakpoints]; // Array to store the number of pixels for each controller
    for (int i = 0; i < num_breakpoints; i++) {
        breakpoints_length[i] = breakpoints[i]; 
    }
    
    this->num_breakpoints = num_breakpoints; // Eg, 2 controllers there are 2 breakpoints
    this->targetIP = targetIP; // 169.254.255.255
    this->targetPort = targetPort; // 4628
    this->localPort = localPort; // 8200 (no use in the dll)
    this->bufferSize = bufferSize; // 1500, the length of the message (no use in the dll)
}

/////////////////////////////////////////// Hardware API for controller (Called by middelAPI) //////////////////////////////////////////////
void Hardwaredriver::displayFrame(int** input_colorframe) {

    // Input is the whole color frame 2D array

    // init all the temp variables
    int temp_start = 0;
    int controller_no = 0;
    int start_and_end = 0;
    // cout << "Hardware display" << endl;

    // print "num_breakpoints: " << num_breakpoints << endl;
    // cout << "num_of_controller_used: " << num_of_controller_used << endl;
    // cout << "num_breakpoints: " << num_breakpoints << endl;

    // Send start frame message to state the start of a frame message
    send_startframe(std::wstring(targetIP.begin(), targetIP.end()).c_str(), targetPort, start_and_end);

    // Break the 2D array into smaller arrays based on the breakpoints, then send the control signals
    for (int i = 0; i < num_breakpoints; i++) {
        int** broken_frames = new int*[max_rows];
        for (int j = 0; j < max_rows; j++) {
            broken_frames[j] = new int[breakpoints_length[i]];
            for (int k = 0; k < breakpoints_length[i]; k++) {
                // cout << "at j = " << j << " k = " << k << input_colorframe[j][temp_start + k] << endl;
                broken_frames[j][k] = input_colorframe[j][temp_start + k];
            }
        }

        // /*
        // print broken_frames
        for (int j = 0; j < max_rows; j++) {
            for (int k = 0; k < breakpoints_length[i]; k++) {
                std::cout << broken_frames[j][k] << " ";
            }
            std::cout << std::endl;
        }
        // */

        // cout << "Inside display frame: targetIP: " << targetIP << endl;
        send_controlnum(std::wstring(targetIP.begin(), targetIP.end()).c_str(), targetPort, 
                        breakpoints_length[controller_no], max_rows, controller_no);
        send_controllight_oneframe(std::wstring(targetIP.begin(), targetIP.end()).c_str(), 
                            targetPort, num_of_controller_used, controller_no, broken_frames, max_rows, breakpoints_length[i]);

        temp_start += breakpoints_length[i];
        controller_no++;
    }

    // Send end frame message to state the end of a frame message
    send_endframe(std::wstring(targetIP.begin(), targetIP.end()).c_str(), targetPort, start_and_end);
}


void Hardwaredriver::send_broadcast(const wchar_t* broadcastIP, int targetPort) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::wcerr << L"Failed to initialize Winsock" << std::endl;
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::wcerr << L"Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }

    // Enable broadcasting
    BOOL broadcast = TRUE;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR) {
        std::wcerr << L"Failed to set socket option for broadcasting" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(targetPort);

    

    // Logging targetIP for debugging
    if (broadcastIP == nullptr) {
        std::cout << "targetIP is null" << std::endl;
    } else {
        std::wcout << L"Attempting to connect to IP: " << broadcastIP << std::endl;
    }


    // Use InetPtonW for wide character strings
    if (InetPtonW(AF_INET, broadcastIP, &(servaddr.sin_addr)) != 1) {
        std::wcerr << L"Failed to set up server address" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    /*
    ======================================================
        THE COMMAND TO SEND BROADCAST MESSAGE
    ======================================================
    */

    // Define the command buffer
    std::vector<unsigned char> command;

    // Add the command
    command.push_back (0x56);

    // Create two random hex bytes
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 255);
    for (int i = 0; i < 2; ++i) { // Note: Changed loop to start from 0 for vector indexing
        unsigned char randomByte = static_cast<unsigned char>(dis(gen));
        command.push_back(randomByte); // Use push_back for vectors
    }

    // Add the valid length (len(PC/控制機 + 數據 1 - 14))
    command.push_back(0x00);
    command.push_back(0x0e);

    // Add PC
    command.push_back(0x02);

    // Add 數據
    std::string temp = "KX-HC-8";
    for (char c : temp) {
        command.push_back(static_cast<unsigned char>(c));
    }

    // Fill in the gap if any, between the end of temp and 數據8
    while (command.size() < 13) {
        command.push_back(0x00);
    }

    // Add 數據8
    command.push_back(0x04);

    // Add 數據9
    command.push_back(0x01);

    // Add 數據10
    command.push_back(0x00);

    // Add 數據11 - 14 (write the subnet mask of the device)
    // need find how to find the subnet mask in its own pc !!
    command.push_back(0xFF);
    command.push_back(0xFF);
    command.push_back(0x00);
    command.push_back(0x00);

    // Add the length (21)
    command.push_back(0x00);
    command.push_back(0x15);

    // Calculate and add the CRC
    unsigned char checksum = Check_sum(command.data(), command.size() + 1); // Assuming Check_sum takes pointer and size
    unsigned char crc = find_crc(checksum); // Assuming find_crc calculates CRC
    command.push_back(crc);
    
    std::string message(reinterpret_cast<char*>(command.data()), command.size());

    // The line below is sending the message
    if (sendto(sockfd, message.c_str(), message.length(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to send broadcast message" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return ;
    }

    // print the message in hexcdecimal
    for (auto c : command) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;

    closesocket(sockfd);
    WSACleanup();
}

////////////////////////////////////////////// Hardware API for receiver ///////////////////////////////////////////
// vector<unsigned char> receiveMessage(int localport, int bufferSize)
// return a vecotr of unsigned char for about  1315 bytes
// Update: Will not call by unity
bool** Hardwaredriver::getStepped() {

    // Create a 2D dynamic array to store the received data where all are false
    bool** received_data = new bool*[max_rows];
    for (int i = 0; i < max_rows; i++) {
        received_data[i] = new bool[max_cols];
        for (int j = 0; j < max_cols; j++) {
            received_data[i][j] = false;
        }
    }

    // /*
    int temp_flag = 3;
    for (int i = 0; i < num_of_controller_used; i++) {

        // Receive the message
        std::vector<int> received_message = receiveBroadcastSignal(localPort, bufferSize);

        // print recevied_message out
        for (int i = 0; i < received_message.size(); i++) {
            std::cout << received_message[i] << " ";
        }

        // change received_message[1] to decimal and use a variable to store it
        int receiver_no = static_cast<int>(received_message[1]);

        // drop the first 2 bytes
        received_message.erase(received_message.begin(), received_message.begin() + 2);

        // bug !!!
        // need to fill in different space
        // for each port
        for (int j = 0; j < breakpoints_length[i]; j++) {
            for (int k = 0; k < max_cols; k++) {
                int receive_message_no = j * 170 + k;
                // if the bit is 0xab, set the corresponding LED to be true
                if (received_message[receive_message_no] == 0xab) {
                    received_data[j][k + 4 * receiver_no] = true;
                } else {
                    received_data[j][k + 4 * receiver_no] = false;
                }
                // received_data[j][k] = received_message[receive_message_no];
            }
        }
    }

    // // Create a const version of the array
    // const bool** const_received_data = const_cast<const bool**>(received_data);


    // */

    // return the received_data
    return received_data;

    
}



//////////////////////////// Hardware API for controller (Not directly called by middleAPI) ////////////////////////////////////////////
void Hardwaredriver::send_startframe(const wchar_t* targetIP, int targetPort, int controller_no) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::wcerr << L"Failed to initialize Winsock" << std::endl;
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::wcerr << L"Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(targetPort);

    // Logging targetIP for debugging
    if (targetIP == nullptr) {
        std::cout << "targetIP is null" << endl;
    }else
    std::wcout << L"Attempting to connect to IP: " << targetIP << std::endl;

    // Use InetPtonW for wide character strings
    if (InetPtonW(AF_INET, targetIP, &(servaddr.sin_addr)) != 1) {
        std::wcerr << L"Failed to set up server address" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    /*
    ======================================================
        THE COMMAND TO SEND START FRAME MESSAGE
    ======================================================
    */

    std::vector<unsigned char> command;

    // Define the command to send
    command.push_back(0x90); 

    // Create two random hex bytes
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 255);
    for (int i = 1; i <= 2; ++i) {
        command.push_back(static_cast<unsigned char>(dis(gen)));
    }

    // Add fixed elements
    // command.insert(command.end(), {0x00, 0x09, 0x02, 0x00, 0x00, 0x33, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E});


    // 有效長度
    command.push_back(0x00);
    command.push_back(0x09);

    // PC or 控制機
    command.push_back(0x02);

    // Controller number
    command.push_back(static_cast<unsigned char>(controller_no));

    // Port number
    // command.push_back(static_cast<unsigned char>(port_no));
    command.push_back(0x00);
    
    // Start Frame
    command.push_back(0x33);
    command.push_back(0x44);

    // Use 0x00 to fill the command
    for (int i = 0; i < 4; ++i) {
        command.push_back(0x00);
    }

    // Toatl length
    command.push_back(0x00);
    command.push_back(0x0E);

    // Calculate CRC and add it
    unsigned char checksum = Check_sum(command.data(), command.size());
    unsigned char crc = find_crc(checksum);
    command.push_back(crc);

    if (sendto(sockfd, reinterpret_cast<char*>(command.data()), command.size(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to send command" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    // print the message in hexadecimal
    std::cout << "Start Frame message: ";
    for (auto c : command) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;

    closesocket(sockfd);
    WSACleanup();
}

// no_of_port_used is equals to breakpoints length
void Hardwaredriver::send_controlnum(const wchar_t* targetIP, int targetPort, int no_of_port_used, int max_rows, int controller_no) {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::wcerr << L"Failed to initialize Winsock" << std::endl;
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::wcerr << L"Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(targetPort);

    // Use InetPtonW for wide character strings
    if (InetPtonW(AF_INET, targetIP, &(servaddr.sin_addr)) != 1) {
        std::wcerr << L"Failed to set up server address" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    std::vector<unsigned char> command; // Use vector for dynamic size

    // Define the command to send
    command.push_back(0x90);

    // Create two random hex bytes
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 255);
    for (int i = 1; i <= 2; ++i) {
        command.push_back(static_cast<unsigned char>(dis(gen)));
    }

    // 有效長度 len(PC/控制 + 數據), 2B
    command.push_back(0x00);
    command.push_back(0x19);

    // add PC
    command.push_back(0x02);

    // add 數據
    // 數據1, Controller number
    // command.push_back(static_cast<unsigned char>(controller_no));
    command.push_back(0x00);

    // 數據2, Port number
    // command.push_back(static_cast<unsigned char>(port_no));
    command.push_back(static_cast<unsigned char>(controller_no));

    // 數據3 (3 - 4),  表示數據幀
    command.push_back(0x88);
    command.push_back(0x77);

    // 數據4 (5 - 6), 表示實際像素點
    command.push_back(0xFF);
    command.push_back(0xF0);

    //數據5 (7 - 8), length of telling control number
    command.push_back(0x00);
    command.push_back(0x10);

    // Each port pixel number
    for (int i = 0; i < no_of_port_used; ++i) {
        command.push_back(static_cast<unsigned char>((max_rows >> 8) & 0xFF));
        command.push_back(static_cast<unsigned char>(max_rows & 0xFF));
    }

    // Fill in remaining 4 port pixel number with 0x0000 (The controller can have up to 8 ports)
    // If want to use more ports, adjust the loop by uncommenting
    for (int i = 0; i < 8 - no_of_port_used; ++i) {
        command.push_back(0x00);
        command.push_back(0x00);
        // command.push_back(static_cast<unsigned char>((max_rows >> 8) & 0xFF));
        // command.push_back(static_cast<unsigned char>(max_rows & 0xFF));
    }

    // Total length
    command.push_back(0x00);
    command.push_back(0x1E);

    // Calculate CRC and add it
    unsigned char checksum = Check_sum(command.data(), command.size());
    unsigned char crc = find_crc(checksum);
    command.push_back(crc);

    if (sendto(sockfd, reinterpret_cast<char*>(command.data()), command.size(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to send command" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    // print the message in hexadecimal
    std::cout << "Tell control numbers message: ";
    for (auto c : command) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;

    closesocket(sockfd);
    WSACleanup();
}

void Hardwaredriver::send_controllight_oneframe(const wchar_t* targetIP, int targetPort, int num_of_controller_used, int controller_no,
                                int** input_colorframe, int rows, int cols) {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::wcerr << L"Failed to initialize Winsock" << std::endl;
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::wcerr << L"Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(targetPort);

    // Use InetPtonW for wide character strings
    if (InetPtonW(AF_INET, targetIP, &(servaddr.sin_addr)) != 1) {
        std::wcerr << L"Failed to set up server address" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    /*
    ======================================================
        THE COMMAND TO SEND CONTROL LIGHT ONE FRAME
    ======================================================
    */

    std::vector<unsigned char> command;

    // Define the command to send
    command.push_back(0x90);

    // Create two random hex bytes
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 255);
    for (int i = 0; i < 2; ++i) { // Adjusted loop to start from 0
        command.push_back(static_cast<unsigned char>(dis(gen)));
    }

    // 8 is the number of ports of a controller, 3 is the number of byte to represent colors 
    int controllength = 8 * 3 * rows; // total length for light signal
    int validlength = controllength + 9; // 有效長度


    // Add 有效長度 len(PC/控制 + 數據), 2B
    // command.push_back(0x00);
    // command.push_back(0x15);
    command.push_back(static_cast<unsigned char>((validlength >> 8) & 0xFF));
    command.push_back(static_cast<unsigned char>(validlength & 0xFF));

    // Add PC
    command.push_back(0x02);

    // Add 數據, Controller Number
    // command.push_back(static_cast<unsigned char>(controller_no));
    command.push_back(0x00); // 數據1, 主控制編號

    // 數據2, port number
    // command.push_back(static_cast<unsigned char>(port_no));
    command.push_back(static_cast<unsigned char>(controller_no));

    // 數據3 (3 - 4), 表示數據幀
    command.push_back(0x88);
    command.push_back(0x77);

    // 數據4 (5 - 6), No use for here
    command.push_back(0x00);
    command.push_back(0x00);

    // 數據5 (7 - 8), length of control signal -> 24 * pixelNum
    command.push_back(static_cast<unsigned char>((controllength >> 8) & 0xFF));
    command.push_back(static_cast<unsigned char>(controllength & 0xFF));

    // Create vectors for color use, can add more colors if needed
    std::vector<unsigned char> green = {0xff, 0x00, 0x00};
    std::vector<unsigned char> red = {0x00, 0xff, 0x00};
    std::vector<unsigned char> blue = {0x00, 0x00, 0xff};
    std::vector<unsigned char> yellow = {0xff, 0xff, 0x00};
    std::vector<unsigned char> turnoff = {0x00, 0x00, 0x00};

    // Create a vector for the results use
    std::vector<unsigned char> resultcolor;

    // Create color length
    int colorlength = 3;

    for (int i = 0; i < rows; i++) {
        // Create a vector for the results use
        std::vector<unsigned char> tempcolor;

        for (int loop = 0; loop < colorlength; loop++) {
            // Create a vector for the results use
            std::vector<unsigned char> tempcolor_orignal; // ff ff 00 00

                for (int j = 0; j < cols; j++) {
                    // Green
                    if (input_colorframe[i][j] == 0) {
                        tempcolor_orignal.push_back(green[loop]);
                        // std::cout << input_colorframe[i][j];

                        // print row_num and col_num
                        // std::cout << i << " " << j << std::endl;
                    }
                    // Red
                    else if (input_colorframe[i][j] == 1) {
                        tempcolor_orignal.push_back(red[loop]);
                        // std::cout << input_colorframe[i][j];
                        
                        // print row_num and col_num
                        // std::cout << i << " " << j << std::endl;
                    }
                    // Blue
                    else if (input_colorframe[i][j] == 2) {
                        tempcolor_orignal.push_back(blue[loop]);
                        // std::cout << input_colorframe[i][j];
                        
                        // print row_num and col_num
                        // std::cout << i << " " << j << std::endl;
                    }
                    // Yellow
                    else if (input_colorframe[i][j] == 3) {
                        tempcolor_orignal.push_back(yellow[loop]);
                        // std::cout << input_colorframe[i][j];
                        
                        // print row_num and col_num
                        // std::cout << i << " " << j << std::endl;
                    }

                    else if (input_colorframe[i][j] == 4) {
                        tempcolor_orignal.push_back(turnoff[loop]);
                        // std::cout << input_colorframe[i][j];
                        
                        // print row_num and col_num
                        // std::cout << i << " " << j << std::endl;
                    }

                    else {
                        tempcolor_orignal.push_back(0x00);
                    }
                }

                for (int k = 0; k < 8 - cols; k++) {
                    tempcolor_orignal.push_back(0x00);

                }

                // This is used to check if the color command is correct or not

                /*
                // print tempcolor_orignal
                for (char c : tempcolor_orignal) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
                }
                // print endl
                std::cout << std::endl;

                */

                // push the tempcolor to the resultcolor
                tempcolor.insert(tempcolor.end(), tempcolor_orignal.begin(), tempcolor_orignal.end());
            }

            // push the tempcolor to the resultcolor
            resultcolor.insert(resultcolor.end(), tempcolor.begin(), tempcolor.end());
        }

    // Add the result color to the command
    command.insert(command.end(), resultcolor.begin(), resultcolor.end());

    // Total length
    int totalLength = command.size() + 1;
    command.push_back(static_cast<unsigned char>((totalLength >> 8) & 0xFF));
    command.push_back(static_cast<unsigned char>(totalLength & 0xFF));

    // 校檢 CRC
    unsigned char checksum = Check_sum(command.data(), command.size() - 1); // Adjusted to use vector's data method
    unsigned char crc = find_crc(checksum);
    command.push_back(crc);

    if (sendto(sockfd, reinterpret_cast<char*>(command.data()), command.size(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to send command" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    // print the message in hexcdecimal
    std::cout << "Control Lights message: ";
    for (char c : command) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
    }
    std::cout << std::endl;


    closesocket(sockfd);
    WSACleanup();
}

void Hardwaredriver::send_endframe(const wchar_t* targetIP, int targetPort, int controller_no) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::wcerr << L"Failed to initialize Winsock" << std::endl;
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::wcerr << L"Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(targetPort);

    // Use InetPtonW for wide character strings
    if (InetPtonW(AF_INET, targetIP, &(servaddr.sin_addr)) != 1) {
        std::wcerr << L"Failed to set up server address" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    std::vector<unsigned char> command;

    // Define the command to send
    command.push_back(0x90);

    // Create two random hex bytes
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 255);
    for (int i = 0; i < 2; ++i) {
        command.push_back(static_cast<unsigned char>(dis(gen)));
    }

    // 有效長度
    command.push_back(0x00);
    command.push_back(0x09);

    // PC or 控制機
    command.push_back(0x02);

    // Controller number
    command.push_back(static_cast<unsigned char>(controller_no));

    // Port number
    // command.push_back(static_cast<unsigned char>(port_no));
    command.push_back(0x00);
    
    // Start Frame
    command.push_back(0x55);
    command.push_back(0x66);

    // Use 0x00 to fill the command
    for (int i = 0; i < 4; ++i) {
        command.push_back(0x00);
    }

    // Toatl length
    command.push_back(0x00);
    command.push_back(0x0E);

    // Calculate CRC and add it
    unsigned char checksum = Check_sum(command.data(), command.size());
    unsigned char crc = find_crc(checksum);
    command.push_back(crc);

    if (sendto(sockfd, reinterpret_cast<char*>(command.data()), command.size(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to send command" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    // Print the message in hexadecimal
    std::cout << "End Frame message: ";
    for (auto c : command) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;

    closesocket(sockfd);
    WSACleanup();
}

////////////////////////////////////////////// Hardware API for receiver ///////////////////////////////////////////

// No use for now, as receiver has been shifted to unity

// /*
vector<int> Hardwaredriver::receiveBroadcastSignal(int BROADCAST_PORT, int BUFFER_SIZE) {
    std::vector<int> signalData;

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error initializing Winsock" << std::endl;
        return signalData;
    }

    // Create a UDP socket
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        WSACleanup();
        return signalData;
    }

    // Set socket options for broadcast
    BOOL broadcastEnable = TRUE;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastEnable, sizeof(broadcastEnable)) == SOCKET_ERROR) {
        std::cerr << "Error setting socket options for broadcast" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return signalData;
    }

    // Bind the socket to a specific port
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(BROADCAST_PORT);
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Error binding socket" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return signalData;
    }

    // Receive the broadcast signal
    char buffer[1500];
    struct sockaddr_in cliaddr;
    int len = sizeof(cliaddr);
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len);
    if (n == SOCKET_ERROR) {
        std::cerr << "Error receiving broadcast signal" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return signalData;
    }

    // Process the received signal data
    for (int i = 0; i < n; i++) {
        signalData.push_back(static_cast<int>(buffer[i]));
    }

    // Close the socket and clean up Winsock
    closesocket(sockfd);
    WSACleanup();

    return signalData;
}

// */