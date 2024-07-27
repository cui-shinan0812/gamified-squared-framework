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
    int total_controller_no = 1;
    int total_receiver_no = 1;
    
    // Create a Controller Object
    ControllerInfo controller_0;
    controller_0.targetIP = "169.254.255.255";
    controller_0.targetPort = 4628;
    controller_0.controller_no = 0;
    controller_0.num_of_port_used = 4;
    controller_0.total_pixel = 16;
    controller_0.port_pixelNum = new int[controller_0.num_of_port_used];
    for (int i = 0; i < controller_0.num_of_port_used; ++i) {
        controller_0.port_pixelNum[i] = 4;    // 4*4 tiles
    }

    // Create a Receiver Object
    ReceiverInfo receiver_1;
    receiver_1.localPort = 8200;
    receiver_1.bufferSize = 1500;

    std::random_device rd;
    std::mt19937 gencolor(rd());
    std::uniform_int_distribution<> dis(0,2);
    
    // Create a light_info Object
    LightInfo light_info;
    light_info.rows = 4;
    light_info.cols = 4;
    light_info.depth = 1;
    light_info.frame_duration = 5;
    light_info.duration_yellow = 0.3;
    light_info.colorarray = new int**[light_info.depth];
    light_info.step_start_time = new std::chrono::steady_clock::time_point*[light_info.rows];
    light_info.step_state = new bool*[light_info.rows];

    // Create a 3D array for colorarray
    for (int d = 0; d < light_info.depth; d++) {
        light_info.colorarray[d] = new int*[light_info.rows];
        for (int i = 0; i < light_info.rows; ++i) {
            light_info.colorarray[d][i] = new int[light_info.cols];
            for (int j = 0; j < light_info.cols; ++j) {
                light_info.colorarray[d][i][j] = dis(gencolor);
                // light_info.colorarray[d][i][j] =3;
            }
        }
    }

    /*

    light_info.colorarray[0][0][0] = 0; light_info.colorarray[0][0][1] = 3; light_info.colorarray[0][0][2] = 0; light_info.colorarray[0][0][3] = 3;
    light_info.colorarray[0][1][0] = 0; light_info.colorarray[0][1][1] = 3; light_info.colorarray[0][1][2] = 0; light_info.colorarray[0][1][3] = 3;
    light_info.colorarray[0][2][0] = 0; light_info.colorarray[0][2][1] = 3; light_info.colorarray[0][2][2] = 0; light_info.colorarray[0][2][3] = 3;
    light_info.colorarray[0][3][0] = 0; light_info.colorarray[0][3][1] = 3; light_info.colorarray[0][3][2] = 0; light_info.colorarray[0][3][3] = 3;

    */

    for (int i = 0; i < light_info.rows; ++i) {
        light_info.step_state[i] = new bool[light_info.cols];  // For LED 2D step state
        light_info.step_start_time[i] = new std::chrono::steady_clock::time_point[light_info.cols];    // For LED 2D step start time

        for (int j = 0; j < light_info.cols; ++j) {
            light_info.step_state[i][j] = false;    // set all to be false
            light_info.step_start_time[i][j] = std::chrono::steady_clock::time_point(); // set all to be 
        }
    }

    // send the broadcast message
    send_broadcast(controller_0.targetPort);

    int frame_no = 0;

    // write a while loop untill time equals to depth
    while (frame_no < light_info.depth) {

        int step_row = 0;
        int step_col = 0;

        // Get the current time
        auto startTime = std::chrono::steady_clock::now();
        

        int counter = 0;
        while (true) {
            counter++;

            // print the light_info.colorarray[0]
            for (int i = 0; i < light_info.rows; ++i) {
                for (int j = 0; j < light_info.cols; ++j) {
                    std::cout << light_info.colorarray[frame_no][i][j] << " ";
                }
                std::cout << std::endl;
            }

            send_startframe(const_cast<wchar_t*>(std::wstring(controller_0.targetIP.begin(), controller_0.targetIP.end()).c_str()), 
                                                controller_0.targetPort, controller_0.controller_no, 0);
            send_controlnum(const_cast<wchar_t*>(std::wstring(controller_0.targetIP.begin(), controller_0.targetIP.end()).c_str()), 
                                                controller_0.targetPort, 4, controller_0.port_pixelNum, controller_0.controller_no, 0);
            send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(controller_0.targetIP.begin(), controller_0.targetIP.end()).c_str()), 
                                        controller_0.targetPort, controller_0.controller_no, 0, light_info.colorarray[frame_no], light_info.rows, light_info.cols);
            send_endframe(const_cast<wchar_t*>(std::wstring(controller_0.targetIP.begin(), controller_0.targetIP.end()).c_str()), 
                                                            controller_0.targetPort, controller_0.controller_no, 0);

            std::cout << std::endl;

            // break;
            
            // send_startframe(const_cast<wchar_t*>(std::wstring(controller_0.targetIP.begin(), controller_0.targetIP.end()).c_str()), 
            //                                     controller_0.targetPort, controller_0.controller_no, controller_0.num_of_port_used);
            // send_controlnum(const_cast<wchar_t*>(std::wstring(controller_0.targetIP.begin(), controller_0.targetIP.end()).c_str()), targetPort, 4);
            // send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 
            //                             light_info.colorarray[frame_no], rows, cols);
            // send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
            
            // Process the received message and perform the necessary actions
            std::vector<unsigned char> receivedMessage = receiveMessage(receiver_1.localPort, receiver_1.bufferSize);
            // // print the receivedMessage
            // for (int i = 0; i < receivedMessage.size(); ++i) {
            //     std::cout << std::hex << static_cast<int>(receivedMessage[i]) << " ";
            // }

            // Extract the received data from the received message with 3th to 7th index, 10th to 14th index, 17th to 21st index, 24th to 28th index

            std::vector<unsigned char> receivedData_modified;
            int temp_flag = 3;
            for (int i = 0; i < controller_0.num_of_port_used; ++i) {
                // Ensure the end position does not exceed the bounds of receivedMessage
                int end_pos = temp_flag + controller_0.port_pixelNum[i];
                if (end_pos > receivedMessage.size()) {
                    throw std::length_error("Attempted to access beyond the end of receivedMessage");
                }
                receivedData_modified.insert(receivedData_modified.end(), receivedMessage.begin() + temp_flag, receivedMessage.begin() + end_pos);
                temp_flag += 171; // Assuming 171 is the correct increment for temp_flag
            }

            // print "Received Data Modified: "
            std::cout << "Received Data Modified: ";

            // print the receivedData_modified
            for (int i = 0; i < receivedData_modified.size(); ++i) {
                std::cout << std::hex << static_cast<int>(receivedData_modified[i]) << " ";
            }

            std::cout << std::endl;
            
            
            // std::vector<unsigned char> receivedData_modified(receivedMessage.begin() + 3, receivedMessage.begin() + 7);

            // // print the receivedData_modified
            // for (int i = 0; i < receivedData_modified.size(); ++i) {
            //     std::cout << std::hex << static_cast<int>(receivedData_modified[i]) << " ";
            // }
 
            // Reshape the receivedData_modified to same size as the frame
            std::vector<std::vector<unsigned char>> reshapedreceivedData_modified(light_info.rows, std::vector<unsigned char>(light_info.cols));

            for (int i = 0, k = 0; i < light_info.rows; ++i) {
                for (int j = 0; j < light_info.cols; ++j, ++k) {
                    reshapedreceivedData_modified[i][j] = receivedData_modified[k];
                }
            }

            // if receivedData_modified has value not equal to 0x00, give the position of the value
            for (int i = 0; i < light_info.rows; ++i) {
                for (int j = 0; j < light_info.cols; ++j) {
                    // step on blue tile and shut down
                    if (reshapedreceivedData_modified[i][j] != 0x00 && 
                        light_info.step_state[i][j] == false && 
                        light_info.colorarray[frame_no][i][j] == 2) {
                        // turn it off
                        light_info.colorarray[frame_no][i][j] = 4;
                    }

                    // step on red tile and yellow will shark
                    if (reshapedreceivedData_modified[i][j] != 0x00 && 
                        light_info.step_state[i][j] == false && 
                        light_info.colorarray[frame_no][i][j] == 1) {
                        auto yellowstepTime = std::chrono::steady_clock::now();
                        light_info.step_start_time[i][j] = yellowstepTime;

                        // change it to yellow
                        light_info.colorarray[frame_no][i][j] = 3;

                        // state the position of the yellow
                        step_row = i;
                        step_col = j;

                        // change the state of already stepped to true
                        light_info.step_state[i][j] = true;
                    }
                }
            }

            // check all the value in step_start_time
            for (int i = 0; i < light_info.rows; ++i) {
                for (int j = 0; j < light_info.cols; ++j) {
                    // check if step_state is 0
                    if (light_info.step_start_time[i][j] == std::chrono::steady_clock::time_point()) {
                        continue;
                    }

                    // record the time how long the yellow stay in the position
                    auto currentTime_yellow = std::chrono::steady_clock::now();
                    auto elapsed_yellow = std::chrono::duration_cast<std::chrono::seconds>(currentTime_yellow - light_info.step_start_time[i][j]);
                    if (elapsed_yellow.count() >= light_info.duration_yellow && light_info.step_state[i][j] == true) {
                        // change the yellow to shut down
                        light_info.colorarray[frame_no][i][j] = 4;




                        // // send the shut down frame
                        // send_startframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
                        // send_controlnum(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort, 4);
                        // send_controllight_oneframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), 
                        //                             targetPort, light_info.colorarray[frame_no], rows, cols);
                        // send_endframe(const_cast<wchar_t*>(std::wstring(targetIP.begin(), targetIP.end()).c_str()), targetPort);
                        // // // print "shut down the yellow"
                        // // std :: cout << "shut down the yellow" << std::endl;


                        light_info.step_start_time[i][j] = std::chrono::steady_clock::time_point();
                    }
                }
            }

            // Check if the frame duration has passed
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

            if (elapsed.count() >= light_info.frame_duration) {
                std::cout << "\n\nbreak:  " << counter << "\n\n";
                counter = 0;

                // resume the step state to false
                for (int i = 0; i < light_info.rows; ++i) {
                    for (int j = 0; j < light_info.cols; ++j) {
                        light_info.step_state[i][j] = false;
                    }
                }

                // resume the step time to be 0
                for (int i = 0; i < light_info.rows; ++i) {
                    for (int j = 0; j < light_info.cols; ++j) {
                        light_info.step_start_time[i][j] = std::chrono::steady_clock::time_point();
                    }
                }
                
                break;

            }
        }

        frame_no++;
    }

    return 0;
}