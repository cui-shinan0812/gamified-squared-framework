#ifdef UNITY_EXPORTS
#define UNITY_API __declspec(dllexport)
#else
#define UNITY_API __declspec(dllimport)
#endif

#include <vector> // Add this line to include the vector header
#include <string> // Add this line to include the string header



extern "C" {
	UNITY_API void init(int m, int n, int numPorts, int length, int controller_used, int* portsDistribution, const char*** configMap);
	UNITY_API void destroy();
	UNITY_API void displayFrameUnity(int const* const* frame);
	UNITY_API bool** getSensors(bool ** hardwareMatrix);
}