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
    std::vector<std::string> unique_materials; // Array of fixed size

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


    // Initialize unique materials
    unsigned int unique_materials_count = 0;
    if (!UniqueElements(unique_materials, &unique_materials_count)) {
        ErrorMessage(file_output.c_str(), 2);
        printf("Memory allocation for unique materials has failed\n");
        return 2;
    }
    printf("Loaded the following %u materials:\n", unique_materials_count);
    for (unsigned int i = 0; i < unique_materials_count; ++i) {
        printf("%s\n", unique_materials[i].c_str());
    }
    printf("\n");


    // Read user input
    if (!ReadFromFile(file_input, beginner, material, tool, tool_teeth, job_quality, out_unit, checklist, supported_materials_list)) {
        ErrorMessage(file_output.c_str(), 3);
        printf("Failed to read from file.\n");
        return 3;
    }


    // Clean and extract numerical values
    std::string tool_unit = tool;
    CleanString(material);
    CleanString(tool_unit);
    float tool_diameter = CleanNumber(tool);
    float tool_z = CleanNumber(tool_teeth);
    float speed = CleanNumber(job_quality);
    CleanString(out_unit);

    printf("Material to cut: %s\n", material.c_str());
    printf("Tool Diameter to cut: %.2f %s\n", tool_diameter, tool_unit.c_str());
    printf("Tool Teeth: %.2f\n", tool_z);
    printf("Job Quality: %s\n", job_quality.c_str());
    printf("Units: %s\n", out_unit.c_str());
    printf("\n");

    return 0;
}