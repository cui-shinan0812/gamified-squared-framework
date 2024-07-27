int M = 0;
int N = 0;
int numberOfPorts = 0;
int maxLength = 0;

int*** unityToHardwareMap = nullptr;
int*** hardwareToUnityMap = nullptr;

/*
XXX: Make the hardware class singleton?

XXX: TO BE IMPLEMENTED
create a object of the hardware class
HardwareClassName hardwareObject = None;
*/
#include "hardwareapi.h"
#include "middleWare.h"
#include <iostream>

Hardwaredriver* instance = nullptr;


/*
======================================================
    THESE FUNCTIONS ARE NOT TO BE CALLED BY UNITY
======================================================
*/

// import the hardware class


// Function to get port index from a character code
int getPortIndex(const char* code) {
    // cout << "code: " << code[0] << endl;
    return static_cast<int>(code[0]) - static_cast<int>('A');
}

int getHardwareIndex(const char* code) {
    int portIndex = 0;
    int i = 1;
    while (code[i] != '\0') {
        portIndex = portIndex * 10 + (code[i] - '0');
        ++i;
    }
    // cout << "portIndex: " << portIndex << endl;
    return portIndex;
}


template <typename T> 
T** returnHardwareMatrix(const T* const* unityMatrix) {
    if (unityToHardwareMap == nullptr) {
        return nullptr;
    }

    // Allocate memory for hardware matrix
    T** hardwareMatrix = new T*[maxLength];
    for (int i = 0; i < maxLength; ++i) {
        hardwareMatrix[i] = new T[numberOfPorts];
    }

    // translate unity matrix to hardware matrix
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            int hardwareIndex = unityToHardwareMap[i][j][0];
            int portIndex = unityToHardwareMap[i][j][1];
            hardwareMatrix[hardwareIndex][portIndex] = unityMatrix[i][j];
        }
    }

    return hardwareMatrix;
}


template <typename T>
T** returnUnityMatrix(const T* const* hardwareMatrix) {
    if (hardwareToUnityMap == nullptr) {
        return nullptr;
    }

    // Allocate memory for unity matrix
    T** unityMatrix = new T*[M];
    for (int i = 0; i < M; ++i) {
        unityMatrix[i] = new T[N];
    }

    // translate hardware matrix to unity matrix
    for (int i = 0; i < maxLength; ++i) {
        for (int j = 0; j < numberOfPorts; ++j) {
            if (hardwareToUnityMap[i][j][0] != -1 && hardwareToUnityMap[i][j][1] != -1) {
                unityMatrix[hardwareToUnityMap[i][j][0]][hardwareToUnityMap[i][j][1]] = hardwareMatrix[i][j];
            }
        }
    }

    return unityMatrix;
}

/*
======================================================
    END OF FUNCTIONS NOT TO BE CALLED BY UNITY
======================================================
*/


/*
======================================================
    THESE FUNCTIONS ARE TO BE CALLED BY UNITY
======================================================
*/

/***
@param configMap: 2D array for string (i.e. char)
*/
extern "C" void init(int m, int n, int numPorts, int length, int controller_used, int* portsDistribution ,const char*** configMap) {
    M = m;
    N = n;
    numberOfPorts = numPorts; // how many ports are used in total
    // print numbOfPorts
    // cout << "numPorts: " << numPorts << endl;
    maxLength = length;
    // cout << "length: " << length << endl;
    // cout << "enter init" << endl;

    /* 
    XXX: TO BE IMPLEMENTED
    call the hardware function to initialize the mappings
    use the portsDistribution array to get the number of ports for each controller
    */

   /*
   XXX: TO BE IMPLEMENTED
   initialize the Hardware class object
   if (hardwareObject == None) {
       hardwareObject = new HardwareClassName();
   }
   */

    // Create a string for the target IP

    // initialize the Hardware class object
    string targetIP = "169.254.255.255";
    int targetPort = 4628;
    // Receiver info
    int localPort = 8200;
    int bufferSize = 1500;
    int num_breakpoints = 1;

    instance = new Hardwaredriver(controller_used, maxLength, numPorts, portsDistribution, 
                                    num_breakpoints, targetIP, targetPort, localPort, bufferSize);

    // cout << "create instance" << endl;

    // Allocate memory for unityToHardwareMap
    unityToHardwareMap = new int**[M];
    for (int i = 0; i < M; ++i) {
        unityToHardwareMap[i] = new int*[N];
        for (int j = 0; j < N; ++j) {
            unityToHardwareMap[i][j] = new int[2];
            unityToHardwareMap[i][j][0] = -1;
            unityToHardwareMap[i][j][1] = -1;
        }
    }
    // print shape of unityToHardwareMap
    // cout << "shape of unityToHardwareMap: " << M << " " << N << endl;
    // cout << "after allocate memory for unityToHardwareMap" << endl;
    // Allocate memory for hardwareToUnityMap
    hardwareToUnityMap = new int**[maxLength];
    for (int i = 0; i < maxLength; ++i) {
        hardwareToUnityMap[i] = new int*[numberOfPorts];
        for (int j = 0; j < numberOfPorts; ++j) {
            hardwareToUnityMap[i][j] = new int[2];
            hardwareToUnityMap[i][j][0] = -1; 
            hardwareToUnityMap[i][j][1] = -1;
        }
    }

    // print shape of hardwareToUnityMap
    // cout << "shape of hardwareToUnityMap: " << maxLength << " " << numberOfPorts << endl;
    // cout << "after allocate memory for hardwareToUnityMap" << endl;

    // Initialize unityToHardwareMap and hardwareToUnityMap
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            // print i and j
            // cout << "i: " << i << endl;
            // cout << "j: " << j << endl;
            // cout << "after line 1" << endl;
            int portIndex = getPortIndex(configMap[i][j]);
            // cout << "after line 2" << endl;
            int hardwareIndex = getHardwareIndex(configMap[i][j]);
            // cout << "after line 3" << endl;
            unityToHardwareMap[i][j][0] = hardwareIndex;
            // cout << "after line 4" << endl;
            unityToHardwareMap[i][j][1] = portIndex;
            // cout << "after line 5" << endl;
            // cout << endl
            //      << "hardwareIndex: " << hardwareIndex << endl;
            // cout << "portIndex: " << portIndex << endl;

            hardwareToUnityMap[hardwareIndex][portIndex][0] = i; // bug
            // cout << "after line 6" << endl;
            hardwareToUnityMap[hardwareIndex][portIndex][1] = j;
            // cout << "after line 7" << endl;
            // cout << endl;
        }
    }

    // // print unityToHardwareMap
    // cout << "unityToHardwareMap: " << endl;
    // for (int i = 0; i < M; ++i) {
    //     for (int j = 0; j < N; ++j) {
    //         cout << unityToHardwareMap[i][j][0] << " " << unityToHardwareMap[i][j][1] << endl;
    //     }
    // }

    // cout << "after initialize" << endl;
}

extern "C" void destroy() {
    // Deallocate memory for unityToHardwareMap
    if (unityToHardwareMap != nullptr) {
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                delete[] unityToHardwareMap[i][j];
            }
            delete[] unityToHardwareMap[i];
        }
        delete[] unityToHardwareMap;
        unityToHardwareMap = nullptr;
    }

    // Deallocate memory for hardwareToUnityMap
    if (hardwareToUnityMap != nullptr) {
        for (int i = 0; i < maxLength; ++i) {
            for (int j = 0; j < numberOfPorts; ++j) {
                delete[] hardwareToUnityMap[i][j];
            }
            delete[] hardwareToUnityMap[i];
        }
        delete[] hardwareToUnityMap;
        hardwareToUnityMap = nullptr;
    }

    M = 0;
    N = 0;
    numberOfPorts = 0;
    maxLength = 0;
}

extern "C" void displayFrameUnity(int const* const* frame) {
    int** hardwareMatrix = returnHardwareMatrix(frame);

    // Print hardwareMatrix
    // cout << "hardwareMatrix: " << endl;
    for (int i = 0; i < maxLength; ++i) {
        for (int j = 0; j < numberOfPorts; ++j) {
            cout << hardwareMatrix[i][j] << " ";
        }
        cout << endl;
    }

    /*
    XXX: TO BE IMPLEMENTED
    call the hardware function to display the frame
    */
    instance->displayFrame(hardwareMatrix);
    // print "finished"
    // cout << "finished" << endl;
}

extern "C" bool** getSensors() {
    const bool** hardwareMatrix;
    /*
    XXX: TO BE IMPLEMENTED
    call the hardware function to get the sensors
    */
    hardwareMatrix = instance->getStepped();
    return returnUnityMatrix(hardwareMatrix);
}