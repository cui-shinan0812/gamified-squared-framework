#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <random>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <iomanip>
#include "control_functions.h"

extern "C" void send_endframe(wchar_t* targetIP, int targetPort) {
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
    command.insert(command.end(), {0x00, 0x09, 0x02, 0x00, 0x00, 0x55, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E});

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

    // // Print the message in hexadecimal
    // std::cout << "End Frame message: ";
    // for (auto c : command) {
    //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    // }
    // std::cout << std::endl;

    closesocket(sockfd);
    WSACleanup();
}
