#include "control_functions.h"
#include "middleWare.h"
#include "hardwareapi.h"
#include <windows.h>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <csignal>
using namespace std;


int main() {
    int M = 3;
    int N = 4;
    int numOfPorts = 4;
    int maxLength = 5;
    int *portsDistribution = new int[4]{numOfPorts};

    const char* distribution[M][N] = {
        {"A1", "B1", "C1", "D1"},
        {"A2", "B2", "C2", "C5"},
        {"A3", "A4", "C3", "C4"}
    };

    // create a 2D cstring dynamic array
    const char*** configMap = new const char**[M];
    for (int i = 0; i < M; ++i) {
        configMap[i] = new const char*[N];
        for (int j = 0; j < N; ++j) {
            configMap[i][j] = distribution[i][j];
        }
    }
    
    // Create test_unity
    int**test_unity = new int*[M];
    for (int i = 0; i < M; ++i) {
        test_unity[i] = new int[N];
        for (int j = 0; j < N; ++j) {
            test_unity[i][j] = 0;
        }
    }

    // Print "Start"
    cout << "Start" << endl;

    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            cout << configMap[i][j] << endl;
        }
    }

    // Call the displayFrame function
    init(M, N, numOfPorts, maxLength, 1, portsDistribution, configMap);
    cout << "enter func" << endl;
    displayFrame(test_unity);

    return 0;
}