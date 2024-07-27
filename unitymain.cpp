#include <iostream>
#include <string>
#include "middleWare.h"
using namespace std;

int main() {
    int M = 3;
    int N = 4;
    int numOfPorts = 4;
    int maxLength = 5;
    int *portsDistribution = new int[1]{numOfPorts};

    const char* distribution[M][N] = {
        {"A0", "B0", "C0", "D0"},
        {"A1", "B1", "C1", "C4"},
        {"A2", "A3", "C2", "C3"}
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

    // // Print "Start"
    // cout << "Start" << endl;

    // for (int i = 0; i < M; ++i)
    // {
    //     for (int j = 0; j < N; ++j)
    //     {
    //         cout << configMap[i][j] << endl;
    //     }
    // }

    // Call the displayFrame function
    init(M, N, numOfPorts, maxLength, 1, portsDistribution, configMap);
    // cout << "enter func" << endl;
    displayFrameUnity(test_unity);

    return 0;
}