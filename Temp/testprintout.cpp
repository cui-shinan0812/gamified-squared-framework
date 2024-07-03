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

int main() {
    const std::string targetIP = "169.254.255.255";
    // const std::string targetIP = "169.254.169.94";
    int targetPort = 4628;
    int localPort = 8200;
    int bufferSize = 1500;


    int size = 2;
    // Create a 2d array with random values from 0 to 2
    unsigned char color_state[size][size];

    // Fill the array with random values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 2);

    while (true) {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                color_state[i][j] = static_cast<unsigned char>(dis(gen));
            }
        }

        // Make the 2d array become 1d array
        std::vector<unsigned char> color_input;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                color_input.push_back(color_state[i][j]);
            }
        }

        send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);

        send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);

       send_randomlight22(targetIP, targetPort, 4);

        send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);

        // Call the receiveMessage function with the buffer array
        std::vector<unsigned char> receivedData = receiveMessage(localPort, bufferSize);

        // Print out the received data
        for (unsigned char byte : receivedData) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }

        // get receivedData 4th to 7th byte
        std::vector<unsigned char> receivedData2(receivedData.begin() + 4, receivedData.begin() + 8);

    }

}