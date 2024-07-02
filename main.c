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
#include <ctype.h>      // for character handling functions
#include <math.h>       // for math operations like rounding to the nearest int
#include <stdio.h>      // for standard input/output operations
#include <stdlib.h>     // for memory allocation
#include <string.h>     // for string manipulation functions
#include <strings.h>    // for more string manipulation functions
#include "chipload.h"   // for external user defined functions


// Constant Expressions
#define MAX_UNIT_DISTANCE 3
#define MAX_MATERIAL_DISTANCE 6


int main(void)
 {
    /**
     * Declares and/or inicializes variables for reading user input from a specified .txt file.
     * Variables include flags for beginner status, material, tool unit, tool teeth, job quality, output units,
     * checklist status, and supported materials list status.
     */
    // File name variables
    const char *file_chipload = "ChiploadTable.csv";        // constant char pointer to the name of material information file
    const char *file_input = "SpeedNFeeds.txt";             // constant char pointer to input .txt file
    const char *file_output = "MyTools.txt";                // constant char pointer to output .txt file
    // User input variables (other variables are later declared for processed user input)
    bool begginer = false;                                  // flag, is user a begginer?
    bool checklist = false;                                 // flag, does user want a checklist?
    bool supported_materials_list = false;                  // flag, does user want to see what unique materials are supported?
    char *material = NULL;                                  // empty string (char pointer to NULL) for user input on material
    char *tool = NULL;                                      // empty string (char pointer to NULL) for user input on tool
    char *tool_teeth = NULL;                                // empty string (char pointer to NULL) for user input on tool number of cutting edges
    char *job_quality = NULL;                               // empty string (char pointer to NULL) for user input on job quality (finish vs speed)
    char *out_unit = NULL;                                 // empty string (char pointer to NULL) for user input on what units should be displayed (mm/s? mm/m? inch/s? etc)
    char *unique_materials[N_BUCKETS] = {NULL};             // empty array of strings (array of char pointers to NULL) to store the names of every unique materials loaded into memory
    // Variables for holding .csv values
    float chipload;                                         // float variable for chipload value
    float rpm_factor;                                       // float variable for rpm_factor (feature TO BE added)
    // Arrays
    char *lenght_units[] = {"mm", "in", "inch", "inches"};  // string arrays with supported units for distance and speed (used for convertions and for input string matching)
    char *speed_units[] = {"mm/s", "mm/m", "m/m", "inch/s", "inch/m", "in/s", "in/m", "feet/m"};

    
    /**
     * Loads material and chipload information from a .csv file into a Hash table.
     * If successful, prints "Successfully loaded materials"; otherwise, prints an error message and returns with 1.
     */
    if (Load(file_chipload)) {
        printf("Successfully loaded materials\n");
    }
    else {
        ErrorMessage(file_output, 1);
        printf("Failed to Load materials");
        return 1;
    }
    // for debugging purposes prints the information from the newly created Hash table to stdout
    PrintTable();
    printf("\n");


    /**
     * Initializes an array of unique materials and counts the number of unique materials.
     * If the initialization fails, it prints an error message, and returns with 2.
     */
    if (!UniqueElements(unique_materials, &unique_materials_count)) {
        ErrorMessage(file_output, 2);
        printf("Memory allocation for unique materials has failed\n");
        return 2;
    }
    // for debugging purposes prints the information from the newly created unique materials array to stdout
    printf("Loaded the following %u materials:\n", unique_materials_count);
    for (unsigned int i = 0; i < unique_materials_count; i++)
    {
        printf("%s\n", unique_materials[i]);
    }
    printf("\n");
    

    /**
     * Reads user input data from a specified .txt file and assigns values to corresponding variables.
     * If the read operation fails, it prints an error message, logs the error in file_output, and returns 3.
     */
    if (!ReadFromFile(file_input, &begginer, &material, &tool, &tool_teeth, &job_quality, &out_unit, &checklist, &supported_materials_list)) {
        ErrorMessage(file_output, 3);
        printf("Failed to read from file.");
        return 3;
    }


    /**
     * Clean and extract numerical values from strings representing material, tool unit, tool diameter, tool teeth, job quality, and output units.
     */
    char tool_unit[strlen(tool) + 1];           // The new array has a size equal to the length of 'tool' +1 to accommodate the NULL terminator.
    strcpy(tool_unit, tool);                    // Copies the content of the 'tool' string into a new character array (string) 'tool_unit'
    CleanString(material);
    CleanString(tool_unit);
    float tool_diameter = CleanNumber(tool);
    float tool_z = CleanNumber(tool_teeth);
    float speed = CleanNumber(job_quality);
    CleanString(out_unit);

    // for debugging purposes prints processed user input to stdout
    int a = strlen(material);
    printf("Material to cut: %s, this array is of size %i\n", material, a);
    int b = strlen(tool_unit);
    printf("Tool Diameter to cut: %f %s, this array is of size %i\n", tool_diameter, tool_unit, b);
    printf("Tool Teeth: %f\n", tool_z);
    int e = strlen(job_quality);
    printf("Job Quality: %s, this array is of size %i\n", job_quality, e);
    int f = strlen(out_unit);
    printf("Units: %s, this array is of size %i\n", out_unit, f);
    printf("\n");


    /**
     * Handles error checking for mandatory input strings in the CHIPLOAD CALCULATOR program.
     * Checks for empty or invalid material, tool diameter, tool unit, cutting edges, job quality, and output units.
     * If any of the checks fail, it logs an error message and returns the corresponding error code.
     */
    if (strlen(material) < 1) {
        ErrorMessage(file_output, 4);
        printf("No material selected\n");
        return 4;
    }
    else if ((tool_diameter == 0)) {
        ErrorMessage(file_output, 8);
        printf("No tool diameter selected\n");
        return 8;
    }
    else if (strlen(tool_unit) < 1) {
        ErrorMessage(file_output, 5);
        printf("No tool diameter unit selected\n");
        return 5;
    }
    else if (tool_z > 4 || tool_z <= 0) {
        WarningMessage(file_output, 7);
        printf("No cutting edges selected, resumed with 2\n");
        tool_z = 2; // If tool_z is invalid resume with standard 2 cutting edges with a warning to the user
    }
    else if (speed != 1 && speed != 2 && speed != 3 && speed != 4 && speed != 5) {
        if ((speed == 0)) {
            WarningMessage(file_output, 9);
            printf("No job quality selected, resumed with default case (3)\n");
        }
        else {
            WarningMessage(file_output, 10);
            printf("Not a valid job quality, resumed with default case (3)\n");
        }
    }
    else if (strlen(out_unit) < 1) {
        WarningMessage(file_output, 6);
        printf("No out unit selected, resumed with default mm/m\n");
        out_unit = "mmm"; // If out unit is invalid resume with default mm/m
    }


    /**
     * Convert the tool diameter to millimeters and round it to the nearest integer.
     * 
     * Parameters:
     * - tool_unit: The unit of the tool diameter.
     * - tool_diameter: The original tool diameter value.
     * 
     * Returns:
     * - rounded_diameter: The tool diameter converted to millimeters and rounded to the nearest integer.
     */
    
    char *best_tool_unit = BestMatch(tool_unit, lenght_units, 4, MAX_UNIT_DISTANCE);
    if (strcmp(best_tool_unit, "error") == 0) {
        printf("Invalid entry for the tool diameter\n");
        ErrorMessage(file_output, 11);
        return 11;
    }
    float diameter = Convert(tool_diameter, best_tool_unit, "mm/s");
    int rounded_diameter = round(diameter);
    // for debugging purposes prints the diameter and the rounded diameter
    printf("The diameter is %i %s (rounded from %f)\n", rounded_diameter, best_tool_unit, diameter);
    printf("\n");

    
    /**
     * Check for the best match of a material from a list of unique materials.
     * If the material match is invalid, print an error message, log the error, and return the error code 12.
     */
    char *best_material = BestMatch(material, unique_materials, unique_materials_count, MAX_MATERIAL_DISTANCE);
    if (strcmp(best_material, "error") == 0) {
        printf("Invalid material\n");
        ErrorMessage(file_output, 12);
        return 12;
    }
    // for debugging purposes prints the material that best matches the materials supported
    printf("The best match found in the materials for %s was %s\n", material, best_material);
    printf("\n");


    /**
     * Searches for a material match and retrieves the corresponding chipload and RPM factor.
     * If the Search fails, it logs an error message, prints a notification about the unsupported tool diameter for the valid material,
     * and returns error code 13.
     */
    chipload = 0;
    rpm_factor = 0;
    if (!Search(best_material, rounded_diameter, &chipload, &rpm_factor)) {
        ErrorMessage(file_output, 13);
        printf("For the valid material the tool diameter isn't supported\n");
        return 13;
    }
    // for debugging purposes prints the chipload and rpm_factor that matches the material
    printf("The chipload is %f, and the rpm factor is %f\n", chipload, rpm_factor);
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
    if (begginer) {
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
    char *best_out_unit = BestMatch(out_unit, speed_units, 8, MAX_UNIT_DISTANCE);
    if (strcmp(best_out_unit, "error") == 0) {
        printf("You didn't specify the units you want the results to be displayed, the feedrate was calculated in mm/m.\n");
        WarningMessage(file_output, 14);
        return 14;
    }
    const float feed_rate = Convert(Feeds.y, "mm/m", best_out_unit);
    // for debugging purposes prints the feed rate unit that best matches
    printf("best match for unit %s is %s\n", out_unit, best_out_unit);
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
    printf("The feed_rate is %.1f %s (from the calculated %i mm/m), and the rpm is %i\n", feed_rate, best_out_unit, Feeds.y, Feeds.x);
    printf("\n");


    /**
     * Free allocated memory for unique materials array elements, material, tool unit, tool teeth, job quality, output units,
     * and Unload the Hash table.
     */
    for (unsigned int i = 0; i < unique_materials_count; i++)
    {
        free(unique_materials[i]); // Free the allocated memory
    }
    // Malloced variables
    free(material);
    free(tool);
    free(tool_teeth);
    free(job_quality);
    free(out_unit);
    // Mallocced Hash table
    Unload();


    /**
     * Return with no errors
     */
    printf("Success!\n");
    return 0;
}