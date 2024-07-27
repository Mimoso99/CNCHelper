/**
 * This code represents the main function of a CHIPLOAD CALCULATOR program,
 * that abstracts several steps and considerations from the user returning a suitable feedrate and speed for the CNC machining job.
 * It loads materials from a .csv file, reads and interprets user input from a .txt file, calculates various parameters, 
 * matches tools and materials, and performs necessary unit conversions. 
 * Finally, it prints out the results to a .txt file and frees allocated memory before returning. 
 * It uses the Simplex algorithm, credit attributed to many who thought of it throught history.
 * It uses the Hash function Djb2 by Dan Bernstein for hashing uniformly into a lookup table.
 * It uses the Levenshtein distance algorithm by Vladimir Levenshtein for string matching.
 * LLM GPT3.5 and GPT4.0 was used through chatGPT, CS50 duck and Codiumate in identified instances such as:
 * Debugging, code comment generation (mainly to get a feel for it), general questions such as file organization,
 * for the Makefile and some arguments on .json files.
 * The main sources for learning how to code were the CS50x course, Inês Almeida (senior C/C++ eng.ª),
 * The C programming language, Algorithms.
 */

// Include headers & libraries (<stdbool.h> is included in the header "chipload.h")
#include <iostream>     // for standard C++ library for input and output
#include <string>       // for std::string
#include <cctype>       // for character handling functions
#include <cmath>        // for math operations like rounding to the nearest int
#include <cstdio>       // for standard input/output operations
#include <cstdlib>      // for memory allocation
#include <cstring>      // for string manipulation functions
#include <vector>       // For dynamic array (optional, if needed)
#include "chipload.h"   // for external user defined functions


// Constant Expressions
#define MAX_UNIT_DISTANCE 3
#define MAX_MATERIAL_DISTANCE 6


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
    if (Load(file_chipload.c_str())) {
        printf("Successfully loaded materials\n");
    } else {
        ErrorMessage(file_output.c_str(), 1);
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
    if (!ReadFromFile(file_input.c_str(), &beginner, &material, &tool, &tool_teeth, &job_quality, &out_unit, &checklist, &supported_materials_list)) {
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

    // Error checking
    if (material.empty()) {
        ErrorMessage(file_output.c_str(), 4);
        printf("No material selected\n");
        return 4;
    } else if (tool_diameter == 0) {
        ErrorMessage(file_output.c_str(), 8);
        printf("No tool diameter selected\n");
        return 8;
    } else if (tool_unit.empty()) {
        ErrorMessage(file_output.c_str(), 5);
        printf("No tool diameter unit selected\n");
        return 5;
    } else if (tool_z > 4 || tool_z <= 0) {
        WarningMessage(file_output.c_str(), 7);
        printf("No cutting edges selected, resumed with 2\n");
        tool_z = 2;
    } else if (speed != 1 && speed != 2 && speed != 3 && speed != 4 && speed != 5) {
        if (speed == 0) {
            WarningMessage(file_output.c_str(), 9);
            printf("No job quality selected, resumed with default case (3)\n");
        } else {
            WarningMessage(file_output.c_str(), 10);
            printf("Not a valid job quality, resumed with default case (3)\n");
        }
    } else if (out_unit.empty()) {
        WarningMessage(file_output.c_str(), 6);
        printf("No out unit selected, resumed with default mm/m\n");
        out_unit = "mm/m";
    }

    // Convert tool diameter
    std::string best_tool_unit = BestMatch(tool_unit, length_units, sizeof(length_units) / sizeof(length_units[0]), MAX_UNIT_DISTANCE);
    if (best_tool_unit == "error") {
        printf("Invalid entry for the tool diameter\n");
        ErrorMessage(file_output.c_str(), 11);
        return 11;
    }
    float diameter = Convert(tool_diameter, best_tool_unit, "mm/s");
    int rounded_diameter = std::round(diameter);
    printf("The diameter is %d %s (rounded from %.2f)\n", rounded_diameter, best_tool_unit.c_str(), diameter);
    printf("\n");

    // Find best material match
    std::string best_material = BestMatch(material, unique_materials, unique_materials_count, MAX_MATERIAL_DISTANCE);
    if (best_material == "error") {
        printf("Invalid material\n");
        ErrorMessage(file_output.c_str(), 12);
        return 12;
    }

    chipload = GetChipload(best_material);
    rpm_factor = GetRpmFactor(best_material);

    // Output final results
    printf("Chipload is: %.2f mm/tooth\n", chipload);
    printf("RPM Factor: %.2f\n", rpm_factor);

    return 0;
}