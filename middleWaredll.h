#ifdef UNITY_EXPORTS
#define UNITY_API __declspec(dllexport)
#else
#define UNITY_API __declspec(dllimport)
#endif

#include <vector> // Add this line to include the vector header



extern "C" UNITY_API void init(int m, int n, int numPorts, int length, int controller_used, int* portsDistribution, const char*** configMap);
extern "C" UNITY_API void destroy();
extern "C" UNITY_API void displayFrameUnity(int const* const* frame);
extern "C" UNITY_API bool** getSensors();