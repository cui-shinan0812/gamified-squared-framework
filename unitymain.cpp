#include <iostream>
#include <string>
#include "middleWare.h"
using namespace std;
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>


// This is a simple testing program for the middleAPI and hardwareAPI
int main() {
    const int M = 4;
    const int N = 4;
    int numOfPorts = 4;
    int maxLength = 6;
    int controller_used = 2;
    int *portsDistribution = new int[2]{2, 2};

    const char* distribution[M][N] = {
        {"A0", "B0", "C0", "D0"},
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

    int**test_unity_2 = new int*[M];
    for (int i = 0; i < M; ++i) {
        test_unity_2[i] = new int[N];
        for (int j = 0; j < N; ++j) {
            test_unity_2[i][j] = 1;
        }
    }

    int**test_unity_3 = new int*[M];
    for (int i = 0; i < M; ++i) {
        test_unity_3[i] = new int[N];
        for (int j = 0; j < N; ++j) {
            test_unity_3[i][j] = 0;
        }
    }



    test_unity[0][0] = 1; test_unity[0][1] = 2; test_unity[0][2] = 2; test_unity[0][3] = 1;
    test_unity[1][0] = 0; test_unity[1][1] = 3; test_unity[1][2] = 3; test_unity[1][3] = 0;
    test_unity[2][0] = 1; test_unity[2][1] = 2; test_unity[2][2] = 2; test_unity[2][3] = 1;
    test_unity[3][0] = 0; test_unity[3][1] = 3; test_unity[3][2] = 3; test_unity[3][3] = 0;


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
    init(M, N, numOfPorts, maxLength, controller_used, portsDistribution, configMap);
    // // cout << "enter func" << endl;

    while (true) {
        displayFrameUnity(test_unity);
        Sleep(1000);
        displayFrameUnity(test_unity_2);
        Sleep(1000);
        displayFrameUnity(test_unity_3);
        Sleep(1000);
    }
    displayFrameUnity(test_unity);

    // while (true) {
    //     displayFrameUnity(test_unity);
    //     Sleep(1000);
    // }



    // displayFrameUnity(test_unity);


    // displayFrameUnity(test_unity);
    // // call getSensors
    // bool ** sensors = getSensors();
    // Sleep(1000);

    /*

    // create a 6*4 static array with all false
    int** sensors = new int*[6];
    for (int i = 0; i < 6; ++i) {
        sensors[i] = new int[4];
        for (int j = 0; j < 4; ++j) {
            sensors[i][j] = 0;
        }
    }
    // sensors[0][3] = 1;
    // sensors[1][3] = 1;

    // print sensors
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << sensors[i][j] << " ";
        }
        cout << endl;
    }

    cout << "======================" << endl;

    int ** sensorsPtr = getSensors(sensors);

    // print sensorsPtr
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << sensorsPtr[i][j] << " ";
        }
        cout << endl;
    }

    */

    return 0;

}