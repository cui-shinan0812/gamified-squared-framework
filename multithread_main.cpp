#include "control_functions.h"
#include <windows.h>
#include <iostream>
#include <random>
#include <thread>


int main() {
    std::string targetIP = "169.254.255.255";
    // std::string targetIP = "169.254.169.94";
    int targetPort = 4628;
    int localPort = 8200;
    int bufferSize = 1500;
    
    std::random_device rd;
    std::mt19937 gencolor(rd());
    std::uniform_int_distribution<> dis(0,2);
    
    // Create a 3d dynamic array with random values from 0 to 2, with z-axis from 1-10
    int rows = 2;
    int cols = 2;
    int depth = 100;

    

    int*** colorarray = new int**[rows];
    for (int i = 0; i < rows; ++i) {
        colorarray[i] = new int*[cols];
        for (int j = 0; j < cols; ++j) {
            colorarray[i][j] = new int[depth];
            for (int k = 0; k < depth; ++k) {
                colorarray[i][j][k] = dis(gencolor);
            }
        }
    }

    send_broadcast(targetPort);

    // Create and run threads for sending and receiving messages
    std::thread sendThread(sendMessages_thread, targetIP, targetPort, colorarray, rows, cols, depth);
    std::thread receiveThread(receiveMessages_thread, localPort, bufferSize, targetIP, targetPort, colorarray, rows, cols, depth);
    // Wait for both threads to finish
    sendThread.join();
    receiveThread.join();


    // Free the memory allocated for the 3D array
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            delete[] colorarray[i][j];
        }
        delete[] colorarray[i];
    }
    delete[] colorarray;

    return 0;
}