#ifndef FUNCTONS_H
#define FUNCTONS_H
#include <string>
#include <vector>

unsigned char Check_sum(unsigned char* r_d, unsigned short Len);
unsigned char find_crc(unsigned char out_crc);

void send_broadcast(int targetPort);

void send_startframe(const std::string& targetIP, int targetPort);

void send_controlnum(const std::string& targetIP, int targetPort, int pixelNum);

void send_lightcolor22(const std::string& targetIP, int targetPort, int pixelNum, std::vector<unsigned char> color_input);

void send_endframe(const std::string& targetIP, int targetPort);

std::vector<unsigned char> receiveMessage(int localPort, int bufferSize);

void send_randomlight22(const std::string& targetIP, int targetPort, int pixelNum);

#endif