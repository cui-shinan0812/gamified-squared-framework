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

void send_controlnum(const std::string& targetIP, int targetPort, int pixelNum) {
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
    // 數據1, 主控制編號
    command.push_back(0x00);

    // 數據2
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

    // Port 1 pixel number
    // command.push_back(0x00);
    // command.push_back(0x00);
    command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));

    // Port 2 pixel number
    // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    command.push_back(0x00);
    command.push_back(0x00);

    // Port 3 pixel number
    // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    command.push_back(0x00);
    command.push_back(0x00);

    // Port 4 pixel number
    // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    command.push_back(0x00);
    command.push_back(0x00);

    // Port 5 pixel number
    // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    command.push_back(0x00);
    command.push_back(0x00);

    // Port 6 pixel number
    // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    command.push_back(0x00);
    command.push_back(0x00);

    // Port 7 pixel number
    // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    command.push_back(0x00);
    command.push_back(0x00);

    // Port 8 pixel number
    // command.push_back(static_cast<unsigned char>((pixelNum >> 8) & 0xFF));
    // command.push_back(static_cast<unsigned char>(pixelNum & 0xFF));
    command.push_back(0x00);
    command.push_back(0x00);

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
    // std::cout << "Tell control numbers message: ";
    // for (auto c : command) {
    //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    // }
    // std::cout << std::endl;

    closesocket(sockfd);
    WSACleanup();
}