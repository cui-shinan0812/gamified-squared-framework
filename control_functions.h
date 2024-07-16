#ifndef CONTROL_FUNCTONS_H
#define CONTROL_FUNCTONS_H
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

class LightInfo {
public:
    int rows;
    int cols;
    int depth;
    float frame_duration;
    float duration_yellow;
    int*** colorarray;
    std::chrono::steady_clock::time_point** step_start_time;
    bool** step_state;
};

class ControllerInfo {
public:
    std::string targetIP;
    int targetPort;
    int pixelNum;
    int controller_no;
    int num_of_port_used;
    int total_pixel;
    int *port_pixelNum;
};

class ReceiverInfo {
public:
    int localPort;
    int bufferSize;
};

unsigned char Check_sum(unsigned char* r_d, unsigned short Len);
unsigned char find_crc(unsigned char out_crc);


extern "C" void send_broadcast(int targetPort);

extern "C" void send_startframe(wchar_t* targetIP, int targetPort, int controller_no, int port_no);

extern "C" void send_controlnum(wchar_t* targetIP, int targetPort, int no_of_port_used, int* pixelNum_each_port,
                                int controller_no, int port_no);

extern "C" void send_controllight_oneframe(wchar_t* targetIP, int targetPort, int controller_no, int port_no,
                                            int** input_colorframe, int rows, int cols);

extern "C" void send_endframe(wchar_t* targetIP, int targetPort, int controller_no, int port_no);

extern "C" std::vector<unsigned char> receiveMessage(int localport, int bufferSize);


#endif