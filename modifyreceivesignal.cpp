#include <iostream>
#include <vector>

void processFrame_yellow(int** frame, int rows, int cols, std::vector<unsigned char> receivedData_modified) {
    // Print the receivedData_modified
    for (int i = 0; i < receivedData_modified.size(); ++i) {
        std::cout << std::hex << static_cast<int>(receivedData_modified[i]) << " ";
    }
    std::cout << std::endl;

    // Reshape the receivedData_modified to the same size as the frame
    std::vector<std::vector<unsigned char>> reshapedreceivedData_modified(rows, std::vector<unsigned char>(cols));

    for (int i = 0, k = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j, ++k) {
            reshapedreceivedData_modified[i][j] = receivedData_modified[k];
        }
    }

    // If receivedData_modified has a value not equal to 0x00, update the frame
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (reshapedreceivedData_modified[i][j] != 0x00 && frame[i][j] != 4) {
                frame[i][j] = 3;
            }
        }
    }
}