#ifndef FUNCTONS_H
#define FUNCTONS_H
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>


unsigned char Check_sum(unsigned char* r_d, unsigned short Len);
unsigned char find_crc(unsigned char out_crc);

extern "C" void send_broadcast(int targetPort);

extern "C" void send_startframe(wchar_t* targetIP, int targetPort);

extern "C" void send_controlnum(wchar_t* targetIP, int targetPort, int pixelNum);

extern "C" void send_controllight_oneframe(wchar_t* targetIP, int targetPort, 
                                    int** colorarray, int rows, int cols);

extern "C" void send_endframe(wchar_t* targetIP, int targetPort);

std::vector<unsigned char> receiveMessage(int localPort, int bufferSize);

void send_randomlight22(const std::string& targetIP, int targetPort, int pixelNum);

// void send_randomlight22(wchar_t* targetIP, int targetPort, int pixelNum);

// void sendMessages_thread(const std::string& targetIP, int targetPort, int localPort, 
//                             int bufferSize, int*** colorarray, int rows, int cols, int depth);

// void receiveMessages_thread(int localPort, int bufferSize, int rows, int cols);

// void ReceiveThread(int localPort, int bufferSize);

std::vector<unsigned char> receiveMessage(int port, int bufferSize);

void processFrame_yellow(int** frame, int rows, int cols, std::vector<unsigned char> receivedData_modified);


#endif