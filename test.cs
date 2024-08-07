using System;
using System.Runtime.InteropServices;
using UnityEngine;

public class HardwareAPI : MonoBehaviour
{
    [DllImport("libUnityPlugin")]
    private static extern void init(int m, int n, int numPorts, int length, int controller_used, int[] portsDistribution, string[] configMap);

    [DllImport("libUnityPlugin")]
    private static extern void destroy();

    [DllImport("libUnityPlugin")]
    private static extern void displayFrameUnity(int[] frame);

    [DllImport("libUnityPlugin")]
    private static extern IntPtr getSensors();

 void Start()
    {
        int M = 4;
        int N = 4;
        int numOfPorts = 4;
        int maxLength = 6;

        int[] portsDistribution = new int[] { numOfPorts };

        string[][] distribution = new string[M][];
        distribution[0] = new string[] { "A0", "B0", "C0", "D0" };
        distribution[1] = new string[] { "A1", "B1", "C1", "D1" };
        distribution[2] = new string[] { "A2", "B2", "C2", "C5" };
        distribution[3] = new string[] { "A3", "A4", "C3", "C4" };

        string[] configMap = new string[M * N];
        int index = 0;
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                configMap[index] = distribution[i][j];
                index++;
            }
        }

        int[] testUnity = new int[M * N];

        init(M, N, numOfPorts, maxLength, 1, portsDistribution, configMap);

        displayFrameUnity(testUnity);

        IntPtr sensorsPtr = getSensors();
        bool[,] sensors = new bool[M, N];

        Marshal.Copy(sensorsPtr, sensors, 0, M * N);

        // Print sensors
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                Debug.Log(sensors[i, j] + " ");
            }
            Debug.Log("\n");
        }

        // Clean up memory
        destroy();
    }


    void OnDestroy()
    {
        destroy();
    }

    // You can define other methods to call additional functions here
}