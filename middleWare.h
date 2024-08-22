#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H


#include "hardwareapi.h"
#include <iostream>


int getPortIndex(const char* code);
int getHardwareIndex(const char* code);

template <typename T> 
T** returnHardwareMatrix(const T* const* unityMatrix);

template <typename T>
T** returnUnityMatrix(const T* const* hardwareMatrix);

extern "C" {
    void init(int m, int n, int numPorts, int length, int controller_used, int* portsDistribution, const char*** configMap);
    void destroy();
    void displayFrameUnity(int const* const* frame);
    // bool** getSensors();
    bool ** getSensors(bool const* const* hardwareMatrix);
}

#endif