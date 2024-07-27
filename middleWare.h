#ifndef HARDWARE_API_H
#define HARDWARE_API_H
#include "hardwareapi.h"
#include <iostream>


int getPortIndex(const char* code);
int getHardwareIndex(const char* code);

template <typename T> 
T** returnHardwareMatrix(const T* const* unityMatrix);

template <typename T>
T** returnUnityMatrix(const T* const* hardwareMatrix);

extern "C" void init(int m, int n, int numPorts, int length, int controller_used, int* portsDistribution, const char*** configMap);
extern "C" void destroy();
extern "C" void displayFrameUnity(int const* const* frame);
extern "C" bool** getSensors();

#endif