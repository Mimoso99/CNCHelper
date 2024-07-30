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
    std::vector<std::string> unique_materials; // Array of fixed size

    // Variables for CSV values
    float chipload = 0;
    float rpm_factor = 0;

    // Supported units
    std::vector<std::string> length_units = {"mm", "in", "inch", "inches"};
    std::vector<std::string> speed_units = {"mm/s", "mm/m", "m/m", "inch/s", "inch/m", "in/s", "in/m", "feet/m"};

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
    std::string best_tool_unit = BestMatch(tool_unit, length_units, MAX_UNIT_DISTANCE);
    if (best_tool_unit == "error") {
        printf("Invalid entry for the tool diameter\n");
        ErrorMessage(file_output.c_str(), 11);
        return 11;
    }
    float diameter = Convert(tool_diameter, best_tool_unit, "mm/s");
    int rounded_diameter = std::round(diameter);
    printf("The diameter is %d mm (rounded from %.2f %s)\n", rounded_diameter, diameter, best_tool_unit.c_str());
    printf("\n");


    // Find best material match
    std::string best_material = BestMatch(material, unique_materials, MAX_MATERIAL_DISTANCE);
    if (best_material == "error") {
        printf("Invalid material\n");
        ErrorMessage(file_output.c_str(), 12);
        return 12;
    }
    // for debugging purposes prints the material that best matches the materials supported
    printf("The best match found in the materials for %s was %s\n", material.c_str(), best_material.c_str());
    printf("\n");


    /**
     * Calculates the feed_rate rates based on the given speed value and specific scenarios.
     * 
     * Parameters:
     * - speed: The speed value determining the scenario for feed_rate rate calculation.
     * - chipload: The chipload value used in the feed_rate rate calculation.
     * - tool_z: The number of cutting edges on the tool.
     * 
     * Returns:
     * - Feeds: The calculated feed_rate rates based on the speed scenario.
     */
    if (beginner) {
        speed = 6; // begginer mode
    }
    Point Feeds;          // variable of type Point, holds x(rpm) and y value(feedrate)
    float upper_bound;    // upper bound chipload straight slope
    float lower_bound;    // lower bound chipload straight slope
    switch ((int)speed) {
    case 1: // MAX FINISH
        // lower chipload
        upper_bound = 0.5 * (chipload + MAXDEV) * tool_z;
        lower_bound = 0.5 * (chipload - MAXDEV) * tool_z;
        // Simplex for speed
        Feeds = Simplex(CNCMINSPEED, CNCMAXSPEED, CNCMAXFEED, upper_bound, lower_bound, false);
        break;

    case 2: // FINISH
        // lower chipload
        upper_bound = 0.5 * (chipload + MAXDEV) * tool_z;
        lower_bound = 0.5 * (chipload - MAXDEV) * tool_z;
        // Simplex for speed
        Feeds = Simplex(CNCMINSPEED, CNCMAXSPEED, CNCMAXFEED, upper_bound, lower_bound, false);
        break;

    case 4: // MATERIAL REMOVAL
        // higher chipload
        upper_bound = 0.5 * (chipload + MAXDEV) * tool_z;
        lower_bound = 0.5 * (chipload - MAXDEV) * tool_z;
        // Simplex for feed_rate
        Feeds = Simplex(CNCMINSPEED, CNCMAXSPEED, CNCMAXFEED, upper_bound, lower_bound, false);
        break;

    case 5: // MAX MATERIAL REMOVAL
        // higher chipload
        upper_bound = 0.5 * (chipload + MAXDEV) * tool_z;
        lower_bound = 0.5 * (chipload - MAXDEV) * tool_z;
        // Simplex for feed_rate
        Feeds = Simplex(CNCMINSPEED, CNCMAXSPEED, CNCMAXFEED, upper_bound, lower_bound, false);
        break;

    case 6: // BEGGINER MODE, SIMILAR TO DEFAULT BUT LESS CHIPLOAD (x0.5) AND REDUCED FEEDRATE (x0.625)
        // use chipload from table
        chipload = chipload * tool_z;
        // Midpoint
        Feeds = Midpoint(CNCMINSPEED, CNCMAXSPEED, CNCMAXFEED, chipload);
        Feeds.x = 0.9 * Feeds.x;    // lower rpm
        Feeds.y = 0.5 * Feeds.y;    // lower feedrate significantly
        break;

    default: // BALANCED TOOL LIFE OPTIMIZATION (case 3 or other)
        // use chipload from table
        chipload = chipload * tool_z;
        // Midpoint
        Feeds = Midpoint(CNCMINSPEED, CNCMAXSPEED, CNCMAXFEED, chipload);
        break;
    }

    // Handles edge case where Point Feeds is out of feasible region
    if (Feeds.x == 0 || Feeds.y == 0) {
        WarningMessage(file_output, 15); // Warns user and gives helpful advice
        printf("Chipload out of feasible region\n");
    }


    /**
     * Performs unit conversion for the feed_rate rate based on the desired output unit.
     * 
     * Parameters:
     * - out_unit: The desired output unit for the feed_rate rate.
     * - Feeds.y: The calculated feed_rate rate in mm/m.
     * 
     * Returns:
     * - feed_rate: The converted feed_rate rate based on the desired output unit.
     */
    std::string best_out_unit = BestMatch(out_unit, speed_units, MAX_UNIT_DISTANCE);
    if (strcmp(best_out_unit.c_str(), "error") == 0) {
        printf("You didn't specify the units you want the results to be displayed, the feedrate was calculated in mm/m.\n");
        WarningMessage(file_output, 14);
        return 14;
    }
    const float feed_rate = Convert(Feeds.y, "mm/m", best_out_unit);
    // for debugging purposes prints the feed rate unit that best matches
    printf("best match for unit %s is %s\n", out_unit.c_str(), best_out_unit.c_str());
    printf("\n");


    /**
     * Write the calculated feed_rate rate, speed, and other relevant information to a specified .txt file.
     * 
     * Parameters:
     * - file_output: The name of the output file to write the results to.
     * - best_material: The matched material for the machining job.
     * - tool_diameter: The diameter of the cutting tool.
     * - tool_unit: The unit of the cutting tool diameter.
     * - tool_z: The number of cutting edges on the tool.
     * - speed: The selected job quality/speed for machining.
     * - Feeds: The calculated feed_rate rates based on the speed scenario.
     * - checklist: A flag indicating if a checklist is required.
     * - supported_materials_list: A flag indicating if the supported materials list is needed.
     * 
     * Returns:
     * - 0 if the results were successfully written to the file; otherwise, returns 15 and prints an error message.
     */
    if (!WriteResultsToFile(file_output, best_material, tool_diameter, best_tool_unit, tool_z, speed, Feeds, feed_rate, best_out_unit, unique_materials, checklist, supported_materials_list)) {
        printf("Couldn't write results to file\n");
        return 15;
    }
    // for debugging purposes prints the feed_rate and Point Feeds to stdout
    printf("The feed_rate is %.1f %s (from the calculated %i mm/m), and the rpm is %i\n", feed_rate, best_out_unit.c_str(), Feeds.y, Feeds.x);
    printf("\n");


        /**
     * Free allocated memory for unique materials array elements, material, tool unit, tool teeth, job quality, output units,
     * and Unload the Hash table.
     */
    // Mallocced Hash table
    Unload();


    /**
     * Return with no errors
     */
    printf("Success!\n");
    return 0;


    return 0;
}