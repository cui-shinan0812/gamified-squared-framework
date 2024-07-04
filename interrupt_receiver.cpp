#include <iostream>
#include <vector>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

vector<unsigned char> startUDPServer(int port, int bufferSize) {
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
    serverAddress.sin_port = htons(port);
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

// int main()
// {
//     int port = 8200;
//     int bufferSize = 3000;
//     vector<unsigned char> receivedMessage = startUDPServer(port, bufferSize);
//     for (unsigned char byte : receivedMessage)
//     {
//         cout << static_cast<int>(byte) << " ";
//     }

//     return 0;
// }