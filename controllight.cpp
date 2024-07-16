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

extern "C" void send_controllight_oneframe(wchar_t* targetIP, int targetPort, int controller_no, int port_no,
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
    command.push_back(static_cast<unsigned char>(port_no));
    // command.push_back(0x00); 

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

                tempcolor_orignal.push_back(0x00);
                tempcolor_orignal.push_back(0x00);
                tempcolor_orignal.push_back(0x00);
                tempcolor_orignal.push_back(0x00);

                // // print tempcolor_orignal
                // for (char c : tempcolor_orignal) {
                //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
                // }
                // // print endl
                // std::cout << std::endl;

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

