#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#ifdef BUILDING_DLL
#define HARDWARE_API_H __declspec(dllexport)
#else
#define HARDWARE_API_H __declspec(dllimport)
#endif

#include "hardwareapi.h"
#include <iostream>


int getPortIndex(const char* code);
int getHardwareIndex(const char* code);

template <typename T> 
T** returnHardwareMatrix(const T* const* unityMatrix);

template <typename T>
T** returnUnityMatrix(const T* const* hardwareMatrix);

extern "C" {
    HARDWARE_API_H void init(int m, int n, int numPorts, int length, int controller_used, int* portsDistribution, const char*** configMap);
    HARDWARE_API_H void destroy();
    HARDWARE_API_H void displayFrameUnity(int const* const* frame);
    HARDWARE_API_H bool** getSensors();
}

#endif