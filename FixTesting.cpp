#include <iostream>     // for standard C++ library for input and output
#include <string>       // for std::string
#include <cctype>       // for character handling functions
#include <cmath>        // for math operations like rounding to the nearest int
#include <cstdio>       // for standard input/output operations
#include <cstdlib>      // for memory allocation
#include <cstring>      // for string manipulation functions
#include <vector>       // For dynamic array (optional, if needed)
#include "chipload.h"   // for external user defined functions

int main(void)
{
    // File names
    std::string file_chipload = "ChiploadTable.csv";
    std::string file_input = "SpeedNFeeds.txt";
    std::string file_output = "MyTools.txt";

    // User input
    bool beginner = false;
    bool checklist = false;
    bool supported_materials_list = false;
    std::string material = "";
    std::string tool = "";
    std::string tool_teeth = "";
    std::string job_quality = "";
    std::string out_unit = "";
    std::string unique_materials[N_BUCKETS]; // Array of fixed size

    // Variables for CSV values
    float chipload = 0;
    float rpm_factor = 0;

    // Supported units
    const std::string length_units[] = {"mm", "in", "inch", "inches"};
    const std::string speed_units[] = {"mm/s", "mm/m", "m/m", "inch/s", "inch/m", "in/s", "in/m", "feet/m"};

    // Load material and chipload information
    if (Load(file_chipload)) {
        printf("Successfully loaded materials\n");
    } else {
        ErrorMessage(file_output, 1);
        printf("Failed to Load materials\n");
        return 1;
    }
    PrintTable();
    printf("\n");

    return 0;
}