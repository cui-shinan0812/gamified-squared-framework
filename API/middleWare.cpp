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
// #include "middleWare.h"
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
    cout << "code: " << code[0] << endl;
    return static_cast<int>(code[0]) - static_cast<int>('A');
}

int getHardwareIndex(const char* code) {
    int portIndex = 0;
    int i = 1;
    while (code[i] != '\0') {
        portIndex = portIndex * 10 + (code[i] - '0');
        ++i;
    }
    cout << "portIndex: " << portIndex << endl;
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
@param configMap: 2D array for string (i.e. cha)
*/
void init(int m, int n, int numPorts, int length, int controller_used, int* portsDistribution ,const char*** configMap) {
    M = m;
    N = n;
    numberOfPorts = numPorts; // how many ports are used in total
    // print numbOfPorts
    cout << "numPorts: " << numPorts << endl;
    maxLength = length;
    cout << "length: " << length << endl;
    cout << "enter init" << endl;

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
    instance = new Hardwaredriver(controller_used, M, N, portsDistribution, numPorts, "169.254.255.255");

    cout << "create instance" << endl;

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
    cout << "shape of unityToHardwareMap: " << M << " " << N << endl;
    cout << "after allocate memory for unityToHardwareMap" << endl;
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
    cout << "shape of hardwareToUnityMap: " << maxLength << " " << numberOfPorts << endl;
    cout << "after allocate memory for hardwareToUnityMap" << endl;

    // Initialize unityToHardwareMap and hardwareToUnityMap
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            // print i and j
            cout << "i: " << i << endl;
            cout << "j: " << j << endl;
            // cout << "after line 1" << endl;
            int portIndex = getPortIndex(configMap[i][j]);
            // cout << "after line 2" << endl;
            int hardwareIndex = getHardwareIndex(configMap[i][j]) - 1;
            // cout << "after line 3" << endl;
            unityToHardwareMap[i][j][0] = hardwareIndex;
            // cout << "after line 4" << endl;
            unityToHardwareMap[i][j][1] = portIndex;
            // cout << "after line 5" << endl;
            cout << endl
                 << "hardwareIndex: " << hardwareIndex << endl;
            cout << "portIndex: " << portIndex << endl;

            if (hardwareIndex < 0 || hardwareIndex >= 5) {
                cout << "Error: hardwareIndex is out of bounds." << endl;
                // Handle error, e.g., return a default value or throw an exception
                cout << "HardwareIndex is: " << hardwareIndex << endl;
                return; // Example default value
            }
            if (portIndex < 0 || portIndex >= 4) {
                cout << "Error: portIndex is out of bounds." << endl;
                cout << "PortIndex is: " << portIndex << endl;
                // Handle error
                return; // Example default value
            }


            hardwareToUnityMap[hardwareIndex][portIndex][0] = i; // bug
            // cout << "after line 6" << endl;
            hardwareToUnityMap[hardwareIndex][portIndex][1] = j;
            // cout << "after line 7" << endl;

            cout << endl;
        }
    }

    // print unityToHardwareMap
    cout << "unityToHardwareMap: " << endl;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << unityToHardwareMap[i][j][0] << " " << unityToHardwareMap[i][j][1] << endl;
        }
    }

    cout << "after initialize" << endl;
}

void destroy() {
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

template <typename T> 
void displayFrame(T const* const* frame) {
    T** hardwareMatrix = returnHardwareMatrix(frame);
    /*
    XXX: TO BE IMPLEMENTED
    call the hardware function to display the frame
    */
    cout << "before instance display" << endl;
    if (!instance)
    {
        cout << "instance is null" << endl;
        return;
    }
    instance->displayFrame(hardwareMatrix);
    // print "finished"
    cout << "finished" << endl;
}

bool** getSensors() {
    const bool** hardwareMatrix;
    /*
    XXX: TO BE IMPLEMENTED
    call the hardware function to get the sensors
    */
    hardwareMatrix = instance->getStepped();
    return returnUnityMatrix(hardwareMatrix);
}

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