// Hardwaredriver.h

#ifndef HARDWAREDRIVER_H
#define HARDWAREDRIVER_H

#include <string>
#include <vector>
using namespace std;

// Function declarations for checksum calculation
unsigned char Check_sum(unsigned char* r_d, unsigned short Len);
unsigned char find_crc(unsigned char out_crc);

// Class definition for the Hardware driver
class Hardwaredriver {
// Functions that middleAPI needed to call
public:
    // Constructor with parameters for initializing hardware driver
    Hardwaredriver(int controller_used, int rows, int cols, int *breakpoints, int num_breakpoints, string targetIP, int targetPort,
                                int localPort, int bufferSize);
    // Method to display a frame based on input color frame
    void displayFrame(int** input_colorframe);
    // Method to send a broadcast message to a specified IP address and port
    void send_broadcast(const wchar_t* broadcastIP, int targetPort);
    // Method to retrieve information about stepped frames
    bool** getStepped();

private:
    // Private member variables (Unity does not need to know)
    std::string targetIP;
    int targetPort;
    int num_of_controller_used;
    int max_rows;
    int max_cols;
    int* breakpoints_length;
    int num_breakpoints;
    int localPort;
    int bufferSize;

    // Private methods for sending different frames and control signals
    void send_startframe(const wchar_t* targetIP, int targetPort, int controller_no);
    void send_controlnum(const wchar_t* targetIP, int targetPort, int no_of_port_used, int max_rows, int controller_no);
    void send_controllight_oneframe(const wchar_t* targetIP, int targetPort, int num_of_controller_used, int controller_no,
                                        int** input_colorframe, int rows, int cols);
    void send_endframe(const wchar_t* targetIP, int targetPort, int controller_no);

    // Unity fail to receive signal using dll
    vector<int> receiveBroadcastSignal(int BROADCAST_PORT, int BUFFER_SIZE);
    
};

#endif