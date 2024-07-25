#pragma once
#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <iostream>
#include "hardwareapi.h"

// Define DLL_EXPORT
#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

// Assuming T is a predefined type, include its definition or replace T with the actual type
typedef int T; // Placeholder, replace with actual type definition

// Global variables (consider encapsulating these in a class or struct for better practice)
extern DLL_EXPORT int M, N, numberOfPorts, maxLength;
extern DLL_EXPORT int*** unityToHardwareMap;
extern DLL_EXPORT int*** hardwareToUnityMap;
extern DLL_EXPORT Hardwaredriver* instance; // Assuming Hardwaredriver is a class, include its definition or header

// Function declarations
DLL_EXPORT int getPortIndex(const char* code);
DLL_EXPORT int getHardwareIndex(const char* code);
DLL_EXPORT T** returnHardwareMatrix(const T* const* unityMatrix);
DLL_EXPORT T** returnUnityMatrix(const T* const* hardwareMatrix);
DLL_EXPORT void init(int m, int n, int numPorts, int length, int controller_used, int* portsDistribution, const char*** configMap);
DLL_EXPORT void destroy();
DLL_EXPORT void displayFrame(T const* const* frame);
DLL_EXPORT bool** getSensors();

#ifdef __cplusplus
}
#endif

#endif // MIDDLEWARE_H