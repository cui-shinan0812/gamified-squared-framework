#include <iostream>

int main() {
    // Create a 2D dynamic array with all values set to 0
    int** input_colorframe = new int*[5];
    for (int i = 0; i < 5; ++i) {
        input_colorframe[i] = new int[8];
        for (int j = 0; j < 8; ++j) {
            input_colorframe[i][j] = 0;
        }
    }

    // // Print the array
    // for (int i = 0; i < 5; ++i) {
    //     for (int j = 0; j < 8; ++j) {
    //         std::cout << input_colorframe[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    int* breakpoints_length = new int[3]{1, 3, 4};
    int num_breakpoints = 3;
    int max_rows = 5; // N
    int max_cols = 8; // M

    int temp_start = 0;

    for (int i = 0; i < num_breakpoints; i++) {
        int** broken_frames = new int*[max_rows];
        for (int j = 0; j < max_rows; j++) {
            broken_frames[j] = new int[breakpoints_length[i]];
            for (int k = 0; k < breakpoints_length[i]; k++) {
                broken_frames[j][k] = input_colorframe[j][temp_start + k];
            }
        }
        // Print broken_frames
        for (int j = 0; j < max_rows; j++) {
            for (int k = 0; k < breakpoints_length[i]; k++) {
                std::cout << broken_frames[j][k] << " ";
            }
            std::cout << std::endl;
        }
    temp_start += breakpoints_length[i];

    }
    return 0;
}