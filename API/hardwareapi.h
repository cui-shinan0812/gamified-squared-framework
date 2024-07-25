// Hardwaredriver.h

#ifndef HARDWAREDRIVER_H
#define HARDWAREDRIVER_H

#include <string>
#include <vector>
using namespace std;

class Hardwaredriver {
public:

    Hardwaredriver(int controller_used, int rows, int cols, int* breakpoints, int num_breakpoints, string targetIP);
    void displayFrame(int** input_colorframe);
    void send_broadcast(int targetPort);
    const bool** getStepped();
    

private:
    std::string targetIP;
    int targetPort;
    int num_of_controller_used;
    int max_rows;
    int max_cols;
    int* breakpoints_length;
    int num_breakpoints;
    int localPort;
    int bufferSize;

    void send_startframe(const wchar_t* targetIP, int targetPort, int controller_no);
    void send_controlnum(const wchar_t* targetIP, int targetPort, int no_of_port_used, int max_rows, int controller_no);
    void send_controllight_oneframe(const wchar_t* targetIP, int targetPort, int num_of_controller_used, int controller_no,
                                        int** input_colorframe, int rows, int cols);
    void send_endframe(const wchar_t* targetIP, int targetPort, int controller_no);
    vector<unsigned char> receiveMessage(int localport, int bufferSize);
};

#endif