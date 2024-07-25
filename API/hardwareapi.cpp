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
#include "control_functions.h"
#include "hardwareapi.h"
using namespace std;

/*
int max_rows = 5; // N
int max_cols = 8; // M
*/

// namespace HaredwareDll {

Hardwaredriver::Hardwaredriver(int controller_used, int rows, int cols, int *breakpoints, int num_breakpoints, string targetIP)
{
    num_of_controller_used = controller_used;
    max_rows = rows;
    max_cols = cols;
    breakpoints_length = new int[num_breakpoints];
    for (int i = 0; i < num_breakpoints; i++) {
        breakpoints_length[i] = breakpoints[i];
    }
    num_breakpoints = num_breakpoints;
    this->targetIP = targetIP;
}

/////////////////////////////////////////// Hardware API for controller //////////////////////////////////////////////
void Hardwaredriver::displayFrame(int** input_colorframe) {

    int temp_start = 0;
    int controller_no = 0;
    cout << "Hardware display" << endl;

    for (int i = 0; i < num_breakpoints; i++) {
        int** broken_frames = new int*[max_rows];
        for (int j = 0; j < max_rows; j++) {
            broken_frames[j] = new int[breakpoints_length[i]];
            for (int k = 0; k < breakpoints_length[i]; k++) {
                cout << "at j = " << j << " k = " << k << input_colorframe[j][temp_start + k] << endl;
                broken_frames[j][k] = input_colorframe[j][temp_start + k];
            }
        }

        // print broken_frames
        for (int j = 0; j < max_rows; j++) {
            for (int k = 0; k < breakpoints_length[i]; k++) {
                std::cout << broken_frames[j][k] << " ";
            }
            std::cout << std::endl;
        }

        cout << "Inside display frame: targetIP: " << targetIP << endl;
        send_startframe(std::wstring(targetIP.begin(), targetIP.end()).c_str(), targetPort, controller_no);
        send_controlnum(std::wstring(targetIP.begin(), targetIP.end()).c_str(), targetPort, breakpoints_length[controller_no], max_rows, controller_no);
        send_controllight_oneframe(std::wstring(targetIP.begin(), targetIP.end()).c_str(), 
                            targetPort, num_of_controller_used, controller_no, broken_frames, max_rows, breakpoints_length[i]);
        send_endframe(std::wstring(targetIP.begin(), targetIP.end()).c_str(), targetPort, controller_no);

        temp_start += breakpoints_length[i];
        controller_no++;
    }
}

void Hardwaredriver::send_broadcast(int targetPort) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize winsock" << std::endl;
        return ;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return ;
    }

    const char* broadcastIP = "255.255.255.255";
    // const char* broadcastIP = "169.254.161.94";

    BOOL broadcastEnable = TRUE;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcastEnable, sizeof(broadcastEnable)) == SOCKET_ERROR) {
        std::cerr << "Failed to enable broadcasting" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return ;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(targetPort);
    if (inet_pton(AF_INET, broadcastIP, &(servaddr.sin_addr)) <= 0) {
        std::cerr << "Failed to set up server address" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return ;
    }

    // Define the command buffer
    // unsigned char command[23];
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
    command.push_back(0x0E);

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
    command.push_back(0x14);

    // Calculate and add the CRC
    unsigned char checksum = Check_sum(command.data(), command.size() + 1); // Assuming Check_sum takes pointer and size
    unsigned char crc = find_crc(checksum); // Assuming find_crc calculates CRC
    command.push_back(crc);
    
    std::string message(reinterpret_cast<char*>(command.data()), command.size());

    if (sendto(sockfd, message.c_str(), message.length(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to send broadcast message" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return ;
    }

    // print the message in hexcdecimal
    std::cout << "Broadcasted message: ";
    for (char c : message) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
    }
    std::cout << std::endl;

    closesocket(sockfd);
    WSACleanup();

    return;
}

////////////////////////////////////////////// Hardware API for receiver ///////////////////////////////////////////
// vector<unsigned char> receiveMessage(int localport, int bufferSize)
// return a vecotr of unsigned char for about  1315 bytes

const bool** Hardwaredriver::getStepped() {

    // Create a 2D dynamic array to store the received data
    bool** received_data = new bool*[max_rows];

    for (int i = 0; i < max_rows; ++i) {
        received_data[i] = new bool[max_cols];  // For LED 2D step state
        for (int j = 0; j < max_cols; ++j) {
                received_data[i][j] = false;    // set all to be false
        }
    }

    int temp_flag = 3;
    for (int i = 0; i < num_of_controller_used; i++) {
            // Receive the message
            std::vector<unsigned char> received_message = receiveMessage(localPort, bufferSize);
            // change received_message[1] to decimal and use a variable to store it
            int receiver_no = static_cast<int>(received_message[1]);

            // drop the first 2 bytes
            received_message.erase(received_message.begin(), received_message.begin() + 2);

        // for each port
            for (int j = 0; j < breakpoints_length[i]; j++) {
                for (int k = 0; k < max_cols; k++) {
                    int receive_message = j * 170 + k;
                    // if the bit is 0xab, set the corresponding LED to be true
                    if (received_message[receive_message] == 0xab) {
                        received_data[j][k] = true;
                    }
                }
            }
        }

    // Create a const version of the array
    const bool** const_received_data = const_cast<const bool**>(received_data);

    // return the received_data
    return const_received_data;
}

// Controller info 
std::string targetIP = "169.254.255.255";
int targetPort = 4628;
// Receiver info
int localPort = 8200;
int bufferSize = 1500;


// Size info
int max_rows;
int max_cols;
int num_of_controller_used;
int num_breakpoints;
int *breakpoints_length = nullptr;

//////////////////////////////////////////// Hardware API for controller ////////////////////////////////////////////
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

    // std::stringstream stream;
    // stream << std::hex << std::setw(4) << std::setfill('0') << pixelNum;
    // std::string pixelNumHex = stream.str();
    // // print out pixelNumHex
    // std::cout << "pixelNumHex: " << pixelNumHex << std::endl;

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
    command.push_back(static_cast<unsigned char>(controller_no));

    // 數據2, Port number
    // command.push_back(static_cast<unsigned char>(port_no));
    command.push_back(0x00);

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

    // Fill in remaining 4 port pixel number with 0x00
    for (int i = 0; i < 8 - no_of_port_used; ++i) {
        command.push_back(0x00);
        command.push_back(0x00);
    }

    // // Port 1 pixel number
    // command.push_back(0x00);
    // command.push_back(0x02);
    // // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));

    // // Port 2 pixel number
    // // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    // command.push_back(0x00);
    // command.push_back(0x02);

    // // Port 3 pixel number
    // // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    // command.push_back(0x00);
    // command.push_back(0x00);

    // // Port 4 pixel number
    // // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    // command.push_back(0x00);
    // command.push_back(0x00);

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
    command.push_back(static_cast<unsigned char>(controller_no));
    // command.push_back(0x00); // 數據1, 主控制編號

    // 數據2, port number
    // command.push_back(static_cast<unsigned char>(port_no));
    command.push_back(0x00); 

    // 數據3 (3 - 4), 表示數據幀
    command.push_back(0x88);
    command.push_back(0x77);

    // 數據4 (5 - 6), No use for here
    command.push_back(0x00);
    command.push_back(0x00);

    // 數據5 (7 - 8), length of control signal -> 24 * pixelNum
    command.push_back(static_cast<unsigned char>((controllength >> 8) & 0xFF));
    command.push_back(static_cast<unsigned char>(controllength & 0xFF));


    // command.insert(command.end(), {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                                 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    //                                 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

    // command.insert(command.end(), {0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                                 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    //                                 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

    // command.insert(command.end(), {0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                                 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
    //                                 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00});

    // command.insert(command.end(), {0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
    //                                 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 
    //                                 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00});

    // Create vectors for color use
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
                }

                for (int k = 0; k < 8 - cols; k++) {
                    tempcolor_orignal.push_back(0x00);

                }

                // print tempcolor_orignal
                for (char c : tempcolor_orignal) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
                }
                // print endl
                std::cout << std::endl;

                // push the tempcolor to the resultcolor
                tempcolor.insert(tempcolor.end(), tempcolor_orignal.begin(), tempcolor_orignal.end());
            }

            // // print tempcolor
            // for (char c : tempcolor) {
            //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
            // }

            // push the tempcolor to the resultcolor
            resultcolor.insert(resultcolor.end(), tempcolor.begin(), tempcolor.end());
        }

    // // print resultcolor
    // for (char c : resultcolor) {
    //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
    // }

    // Add the result color to the command
    command.insert(command.end(), resultcolor.begin(), resultcolor.end());

    // Total length
    int totalLength = command.size() + 1;
    command.push_back(static_cast<unsigned char>((totalLength >> 8) & 0xFF));
    command.push_back(static_cast<unsigned char>(totalLength & 0xFF));
    // command.push_back(0x00);
    // command.push_back(0x1b);

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

    // Add fixed bytes as per original structure
    // command.insert(command.end(), {0x00, 0x09, 0x02, 0x00, 0x00, 0x55, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E});

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
vector<unsigned char> Hardwaredriver::receiveMessage(int localport, int bufferSize) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Failed to initialize winsock" << endl;
        return {};
    }

    // Creating socket
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Failed to create socket" << endl;
        WSACleanup();
        return {};
    }

    // Set socket option to enable broadcast
    int broadcastOption = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&broadcastOption), sizeof(broadcastOption)) == SOCKET_ERROR)
    {
        cout << "Failed to set socket option" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return {};
    }

    // Specifying the server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(localport);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Binding socket
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        cout << "Failed to bind socket" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return {};
    }

    // Receiving data
    char buffer[bufferSize];
    memset(buffer, 0, sizeof(buffer));
    sockaddr_in clientAddress;
    int clientAddressLength = sizeof(clientAddress);
    int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "Failed to receive data" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return {};
    }

    // Convert the received data into a vector of unsigned characters
    vector<unsigned char> receivedData(buffer, buffer + bytesReceived);

    // Closing the socket
    closesocket(serverSocket);
    WSACleanup();

    return receivedData;
}



/*

int main() {
    std::random_device rd;
    std::mt19937 gencolor(rd());
    std::uniform_int_distribution<> dis(0,2);

    // Create a 2D dynamic array with all values set to 0
    int** input_colorframe = new int*[5];
    for (int i = 0; i < 5; ++i) {
        input_colorframe[i] = new int[8];
        for (int j = 0; j < 8; ++j) {
            // random from 0 to 2
            input_colorframe[i][j] = dis(gencolor);
        }
    }

    // // Print the array
    // for (int i = 0; i < 5; ++i) {
    //     for (int j = 0; j < 8; ++j) {
    //         std::cout << input_colorframe[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    int num_breakpoints = 3;
    int* breakpoints_length = new int[num_breakpoints]{2, 3, 3};
    int max_rows = 5; // N -> vertical how many
    int max_cols = 8; // M -> horizontal how many, i.e. 
    int controller_used = 3;

    Hardwaredriver hardwaredriver(controller_used, max_rows, max_cols, breakpoints_length, num_breakpoints);

    // send broadcast
    hardwaredriver.send_broadcast(4628);
    hardwaredriver.displayFrame(input_colorframe);

    return 0;
}

*/