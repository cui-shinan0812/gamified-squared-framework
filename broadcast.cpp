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

void send_broadcast(int targetPort) {
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
