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

// Function to send messages in a separate thread
void sendMessages_thread(const std::string& targetIP, int targetPort, int localPort, 
                            int bufferSize, int*** colorarray, int rows, int cols, int depth) {
    std::random_device rd;
    std::mt19937 gencolor(rd());
    std::uniform_int_distribution<> dis(0, 2);

    // Send each frame
    for (int k = 0; k < depth; ++k) {
        // Create a temporary 2D array for the current frame
        int** frame = new int*[rows];
        for (int i = 0; i < rows; ++i) {
            frame[i] = new int[cols];
            for (int j = 0; j < cols; ++j) {
                frame[i][j] = colorarray[i][j][k];
            }
        }

        send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
        send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
        send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, frame, rows, cols);
        send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
        Sleep(500);

        // Free the memory allocated for the temporary frame
        for (int i = 0; i < rows; ++i) {
            delete[] frame[i];
        }
        delete[] frame;
    }
}

// Function to receive messages in a separate thread
void receiveMessages_thread(int localPort, int bufferSize, int rows, int cols) {
    while (true) {
        std::vector<unsigned char> receivedData = receiveMessage(localPort, bufferSize);

        std::vector<unsigned char> receivedData_modified(receivedData.begin() + 3, receivedData.begin() + 7);

        // Reshape the receivedData_modified to the same size as the frame
        std::vector<std::vector<unsigned char>> reshapedreceivedData_modified(rows, std::vector<unsigned char>(cols));

        for (int i = 0, k = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j, ++k) {
                reshapedreceivedData_modified[i][j] = receivedData_modified[k];
            }
        }

        // Process the received data as needed
    }
}
