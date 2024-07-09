#include "control_functions.h"
#include <windows.h>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <csignal>

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
    float duration = 5;
    float duration_yellow = 0.3;

    
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

        // Create a temporary 2D array of with all value inside are bool false
        bool** step_state = new bool*[rows];
        for (int i = 0; i < rows; ++i) {
            step_state[i] = new bool[cols];
            for (int j = 0; j < cols; ++j) {
                step_state[i][j] = false;
            }
        }

        std::chrono::steady_clock::time_point** step_start_time = new std::chrono::steady_clock::time_point*[rows];
        for (int i = 0; i < rows; ++i) {
            step_start_time[i] = new std::chrono::steady_clock::time_point[cols]{std::chrono::steady_clock::time_point()};
        }

        int step_row = 0;
        int step_col = 0;

        // Get the current time
        auto startTime = std::chrono::steady_clock::now();
        

        int counter = 0;
        while (true) {
            counter++;
            send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
            send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
            send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, frame, rows, cols);
            send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
            // Process the received message and perform the necessary actions
            std::vector<unsigned char> receivedMessage = receiveMessage(localPort, bufferSize);
                

            std::vector<unsigned char> receivedData_modified(receivedMessage.begin() + 3, receivedMessage.begin() + 7);
            // // print the receivedData_modified
            // for (int i = 0; i < receivedData_modified.size(); ++i) {
            //     std::cout << std::hex << static_cast<int>(receivedData_modified[i]) << " ";
            // }
 
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
                    // if (reshapedreceivedData_modified[i][j] != 0x00 && frame[i][j] != 4 && step_state[i][j] == false) {

                    // step on blue tile and shut down
                    if (reshapedreceivedData_modified[i][j] != 0x00 && step_state[i][j] == false && frame[i][j] == 2) {
                        // turn it off
                        frame[i][j] = 4;
                    }

                    // step on red tile and yellow will shark
                    if (reshapedreceivedData_modified[i][j] != 0x00 && step_state[i][j] == false && frame[i][j] == 1) {
                        auto yellowstepTime = std::chrono::steady_clock::now();
                        step_start_time[i][j] = yellowstepTime;

                        // change it to yellow
                        frame[i][j] = 3;

                        // state the position of the yellow
                        step_row = i;
                        step_col = j;

                        // change the state of already stepped to true
                        step_state[i][j] = true;
                    }
                }
            }

            // check all the value in step_start_time
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    // check if step_state is 0
                    if (step_start_time[i][j] == std::chrono::steady_clock::time_point()) {
                        continue;
                    }

                    // record the time how long the yellow stay in the position
                    auto currentTime_yellow = std::chrono::steady_clock::now();
                    auto elapsed_yellow = std::chrono::duration_cast<std::chrono::seconds>(currentTime_yellow - step_start_time[i][j]);
                    if (elapsed_yellow.count() >= duration_yellow && step_state[i][j] == true) {
                        // change the yellow to shut down
                        frame[i][j] = 4;

                        // send the shut down frame
                        send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
                        send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
                        send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, frame, rows, cols);
                        send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
                        // // print "shut down the yellow"
                        // std :: cout << "shut down the yellow" << std::endl;
                        step_start_time[i][j] = std::chrono::steady_clock::time_point();
                    }
                }
            }

            // Check if the frame duration has passed
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

            if (elapsed.count() >= duration) {
                std::cout << "\n\nbreak:  " << counter << "\n\n";
                counter = 0;
                break;

            }
        }

        frame_no++;

    }
    return 0;
}