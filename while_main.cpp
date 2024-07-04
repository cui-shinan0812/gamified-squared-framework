#include "control_functions.h"
#include <windows.h>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <vector>

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

    // For counting down
    // Set the duration in seconds
    int duration = 2;

    
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

    int frame_no = 0;

    // write a while loop untill time equals to depth
    while (frame_no < depth) {

    // Create a temporary 2D array of colorarry in depth = 0
    int** frame = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        frame[i] = new int[cols];
        for (int j = 0; j < cols; ++j) {
            frame[i][j] = colorarray[i][j][frame_no];
        }
    }


    // Get the current time
    auto startTime = std::chrono::steady_clock::now();

    while (true) {
        send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
        send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
        send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, frame, rows, cols);
        send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);

        std::vector<unsigned char> receivedMessage = startUDPServer(localPort, bufferSize);
        std::vector<unsigned char> receivedData_modified(receivedMessage.begin() + 3, receivedMessage.begin() + 7);
        // print the receivedData_modified
        for (int i = 0; i < receivedData_modified.size(); ++i) {
            std::cout << std::hex << static_cast<int>(receivedData_modified[i]) << " ";
        }

        // Reshape the receivedData_modified to same size as the frame
        std::vector<std::vector<unsigned char>> reshapedreceivedData_modified(rows, std::vector<unsigned char>(cols));

        for (int i = 0, k = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j, ++k) {
                reshapedreceivedData_modified[i][j] = receivedData_modified[k];
            }
        }

        // if receivedData_modified has value not equal to 0x00, give the position of the value
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (reshapedreceivedData_modified[i][j] != 0x00 && frame[i][j] != 4) {
                    frame[i][j] = 3;
                }
            }
        }

        // change it to yellow
        send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
        send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
        send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, frame, rows, cols);
        send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);

        // for (int i = 0; i < rows; ++i) {
        //         for (int j = 0; j < cols; ++j) {
        //             if (frame[i][j] == 3) {
        //                 frame[i][j] = 4;
        //             }
        //         }
        //     }

        
        
        // send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
        // send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
        // send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, frame, rows, cols);
        // send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);


        // Check if the desired duration has passed
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsed.count() >= duration)
            break;
    
    }

    frame_no++;

    }

        // [1][2]
        // [4][3]
    
        // send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
        // send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
        // send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, frame, rows, cols);
        // send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
        // Sleep(500);


    //     std::vector<unsigned char> receivedData = receiveMessage(localPort, bufferSize);

    //     std::vector<unsigned char> receivedData_modified(receivedData.begin() + 3, receivedData.begin() + 7);

    //     // Reshape the receivedData_modified to same size as the frame
    //     std::vector<std::vector<unsigned char>> reshapedreceivedData_modified(rows, std::vector<unsigned char>(cols));

    //     for (int i = 0, k = 0; i < rows; ++i) {
    //         for (int j = 0; j < cols; ++j, ++k) {
    //             reshapedreceivedData_modified[i][j] = receivedData_modified[k];
    //         }
    //     }
        
    //     // create a bool value if true or not
    //     bool iftouch = false;

    //     // if receivedData_modified has value not equal to 0x00, give the position of the value
    //     for (int i = 0; i < rows; ++i) {
    //         for (int j = 0; j < cols; ++j) {
    //             if (reshapedreceivedData_modified[i][j] != 0x00 && frame[i][j] != 4) {
    //                 frame[i][j] = 3;
    //                 iftouch = true;
    //             }
    //         }
    //     }

    //     if (iftouch) {
    //         send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
    //         send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
    //         send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, frame, rows, cols);
    //         send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
    //         Sleep(1000);

    //         // change the value of the frame to 4 if value inside the frame is equal to 3
    //         for (int i = 0; i < rows; ++i) {
    //             for (int j = 0; j < cols; ++j) {
    //                 if (frame[i][j] == 3) {
    //                     frame[i][j] = 4;
    //                 }
    //             }
    //         }
    //         send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
    //         send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
    //         send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, frame, rows, cols);
    //         send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
    //         Sleep(1000);
            
    //     } 

    //     else {
    //         Sleep(2000);
    //     }
    

    //     // Free the memory allocated for the temporary frame
    //     for (int i = 0; i < rows; ++i) {
    //         delete[] frame[i];
    //     }
    //     delete[] frame;
    // }



        

    // // Free the memory allocated for the 3D array
    // for (int i = 0; i < rows; ++i) {
    //     for (int j = 0; j < cols; ++j) {
    //         delete[] colorarray[i][j];
    //     }
    //     delete[] colorarray[i];
    // }
    // delete[] colorarray;

    

    // send_broadcast(targetPort);

    // while (true) {
    //     send_startframe(targetIP, targetPort);

    //     send_controlnum(targetIP, targetPort, 4);

    //     // send_controllight(targetIP, targetPort);
    //     send_randomlight22(targetIP, targetPort, 4);

    //     send_endframe(targetIP, targetPort);
        
    //     receiveMessage(localPort, bufferSize);
    //     Sleep(1000);
    // }


    // send_startframe(targetIP, targetPort);

    // send_controlnum(targetIP, targetPort, 1);

    // send_randomlight22(targetIP, targetPort, 1);

    // send_endframe(targetIP, targetPort);
    // receiveMessage(localPort, bufferSize);

    return 0;
}