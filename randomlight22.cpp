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

void send_randomlight22(const std::string& targetIP, int targetPort, int pixelNum) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(targetPort);
    if (inet_pton(AF_INET, targetIP.c_str(), &(servaddr.sin_addr)) <= 0) {
        std::cerr << "Failed to set up server address" << std::endl;
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

    int controllength = 24 * pixelNum;
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

    // random choose from 0-2 for variable color
    std::uniform_int_distribution<> dist(0, 2);

    // // Randomly choose a value for color
    // int color = dist(gen);

    for (int j = 0; j < pixelNum; ++j) {
        // Randomly choose a value for color
        int color = dist(gen);

        // Green
        if (color == 0) {
            for (int i = 0; i < 8; ++i) command.push_back(0xff);
            for (int i = 0; i < 16; ++i) command.push_back(0x00);
        }

        // Red
        else if (color == 1) {
            for (int i = 0; i < 8; ++i) command.push_back(0x00);
            for (int i = 0; i < 8; ++i) command.push_back(0xff);
            for (int i = 0; i < 8; ++i) command.push_back(0x00);
        }

        // Blue
        else {
            for (int i = 0; i < 16; ++i) command.push_back(0x00);
            for (int i = 0; i < 8; ++i) command.push_back(0xff);
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

    // print the message in hexcdecimal
    std::cout << "Control Lights message: ";
    for (char c : command) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
    }
    std::cout << std::endl;

    closesocket(sockfd);
    WSACleanup();
}

