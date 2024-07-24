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
Hardwaredriver* instance = nullptr;


/*
======================================================
    THESE FUNCTIONS ARE NOT TO BE CALLED BY UNITY
======================================================
*/

// import the hardware class


// Function to get port index from a character code
int getPortIndex(const char* code) {
    return static_cast<int>(code[0]) - static_cast<int>('A');
}

int getHardwareIndex(const char* code) {
    int portIndex = 0;
    int i = 1;
    while (code[i] != '\0') {
        portIndex = portIndex * 10 + (code[i] - '0');
        ++i;
    }
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

void init(int m, int n, int numPorts, int length, int controller_used, int* portsDistribution ,char*** configMap) {
    M = m;
    N = n;
    numberOfPorts = numPorts; // how many ports are used in total
    maxLength = length;

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

  // initialize the Hardware class object
    instance = new Hardwaredriver(controller_used, M, N, portsDistribution, numPorts);

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

    // Initialize unityToHardwareMap and hardwareToUnityMap
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            int hardwareIndex = getHardwareIndex(configMap[i][j]);
            int portIndex = getPortIndex(configMap[i][j]);
            unityToHardwareMap[i][j][0] = hardwareIndex;
            unityToHardwareMap[i][j][1] = portIndex;
            hardwareToUnityMap[hardwareIndex][portIndex][0] = i;
            hardwareToUnityMap[hardwareIndex][portIndex][1] = j;
        }
    }
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
   instance->displayFrame(hardwareMatrix);
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

    int depth = 10;
    // create a 3D dynamic array 
    char*** configMap = new char**[depth];
    for (int i = 0; i < depth; ++i) {
        configMap[i] = new char*[M];
        for (int j = 0; j < M; ++j) {
            configMap[i][j] = new char[N];
            for (int k = 0; k < N; ++k) {
                configMap[i][j][k] = i * M * N + j * N + k;
            }
        }
    }
    
    // Create test_unity
    int**test_unity = new int*[M];
    for (int i = 0; i < M; ++i) {
        test_unity[i] = new int[N];
        for (int j = 0; j < N; ++j) {
            test_unity[i][j] = i * N + j;
        }
    }


    // Call the displayFrame function
    init(M, N, numOfPorts, maxLength, 1, portsDistribution, configMap);
    displayFrame<int>(test_unity);

    return 0;
}