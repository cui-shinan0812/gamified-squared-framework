#include <iostream>
#include <iomanip>
#include <string>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <vector>

std::vector<unsigned char> receiveMessage(int localPort, int bufferSize) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize winsock" << std::endl;
        return {};
    }

    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return {};
    }

    sockaddr_in recvAddr;
    memset(&recvAddr, 0, sizeof(recvAddr));
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(localPort);
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return {};
    }

    std::vector<unsigned char> buffer(bufferSize, 0);

    std::cout << "Waiting for broadcast message..." << std::endl;

    // Set the socket to non-blocking mode
    u_long mode = 1;
    if (ioctlsocket(sockfd, FIONBIO, &mode) != 0) {
        std::cerr << "Failed to set socket to non-blocking mode" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return {};
    }

    while (true) {
        sockaddr_in senderAddr;
        int senderAddrLen = sizeof(senderAddr);

        int numBytesReceived = recvfrom(sockfd, reinterpret_cast<char*>(buffer.data()), bufferSize - 1, 0, (struct sockaddr*)&senderAddr, &senderAddrLen);
        if (numBytesReceived == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                std::cerr << "Failed to receive broadcast message: " << error << std::endl;
            }
            // Handle other tasks or continue listening
        }
        else {
            buffer.resize(numBytesReceived);  // Resize the buffer to the actual number of received bytes
            closesocket(sockfd);
            WSACleanup();
            return buffer;  // Return the received data as a vector
        }
    }
}