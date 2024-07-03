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


// void send_lightcolor22(const std::string& targetIP, int targetPort, int pixelNum, std::vector<unsigned char> color_input) {
extern "C" void send_controllight_oneframe(wchar_t* targetIP, int targetPort, 
                                            int** colorarray, int rows, int cols) {

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

    int controllength = 24 * rows * cols;
    int validlength = controllength + 9;


    // Add 有效長度 len(PC/控制 + 數據), 2B
    // command.push_back(0x00);
    // command.push_back(0x15);
    command.push_back(static_cast<unsigned char>((validlength >> 8) & 0xFF));
    command.push_back(static_cast<unsigned char>(validlength & 0xFF));

    // Add PC
    command.push_back(0x02);

    // Add 數據
    command.push_back(0x00); // 數據1, 主控制編號

    // 數據2
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

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Green
            if (colorarray[i][j] == 0) {
                for (int k = 0; k < 8; ++k) command.push_back(0xff);
                for (int k = 0; k < 16; ++k) command.push_back(0x00);
            }
            // Red
            else if (colorarray[i][j] == 1) {
                for (int k = 0; k < 8; ++k) command.push_back(0x00);
                for (int k = 0; k < 8; ++k) command.push_back(0xff);
                for (int k = 0; k < 8; ++k) command.push_back(0x00);
            }
            // Blue
            else if (colorarray[i][j] == 2) {
                for (int k = 0; k < 16; ++k) command.push_back(0x00);
                for (int k = 0; k < 8; ++k) command.push_back(0xff);
            }
            // Yellow
            else if (colorarray[i][j] == 3) {
                for (int k = 0; k < 8; ++k) command.push_back(0xff);
                for (int k = 0; k < 8; ++k) command.push_back(0xff);
                for (int k = 0; k < 8; ++k) command.push_back(0x00);
            }

            // White
            else if (colorarray[i][j] == 4) {
                for (int k = 0; k < 8; ++k) command.push_back(0x00);
                for (int k = 0; k < 8; ++k) command.push_back(0x00);
                for (int k = 0; k < 8; ++k) command.push_back(0x00);
            }
        }
}

    // // G1 - G4
    // for (int i = 0; i < 4; ++i) command.push_back(0xff);

    // // R1 - R4
    // for (int i = 0; i < 4; ++i) command.push_back(0x00);

    // // B1 - B4
    // for (int i = 0; i < 4; ++i) command.push_back(0x00);

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

    // // print the message in hexcdecimal
    // std::cout << "Control Lights message: ";
    // for (char c : command) {
    //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
    // }
    // std::cout << std::endl;


    closesocket(sockfd);
    WSACleanup();
}

