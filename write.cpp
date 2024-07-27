/**
 * This file contains the following function definitions for writting results and messages to user output:
 * - ErrorMessage
 * - WriteResultsToFile
 */

// Include headers & libraries (<stdbool.h> is included in the header "chipload.h")
#include <stdio.h>      // for standard input/output operations
#include "chipload.h"   // for external user defined functions


// Initialize error counter (extern if main want's to print number of errors and or warnings)
unsigned int number_errors = 0;
unsigned int number_warnings = 0;


/**
 * ErrorMessage: writes an error message to a file based on the error code provided.
 * 
 * Parameters:
 * @param filename: The name of the file to write the error message to.
 * @param error: The error code to determine the specific error message to write.
 * 
 * Returns:
 * @return This function does not return a value.
 */
void ErrorMessage(const char *filename, int error) {
    FILE *file = fopen(filename, "a");  // open file in append mode
    if (file == NULL) {                 // handles case where file can't be accessed
        printf("Error opening file %s\n", filename); 
        return;
    }

    fprintf(file, "============= ERRORS =============\n\n"); // print error header

    ++number_errors;    // for counting errors (change this in the future, an error returns and the program stops)

    switch (error)      // print error message based on error
    {
    case 1:
        fprintf(file, "ERROR 1: Failed to Load ChiploadTable.csv :(\n\n");
        break;
    
    case 2:
        fprintf(file, "ERROR 2: Failed to allocate memory for materials data structure :(\n\n");
        break;

    case 3:
        fprintf(file, "ERROR 3: Failed to read from file.\n\n");
        break;

    case 4:
        fprintf(file, "ERROR 4: Ups! You forgot to select a material (ex.: Wood or cOrK).\n\n");
        break;
    
    case 5:
        fprintf(file, "ERROR 5: Ups! you forgot the tool diameter unit (ex.: 3mm or 1/4 inch).\n\n");
        break;
    
    case 8:
        fprintf(file, "ERROR 8: UPS! You forgot to specify a tool diameter (ex.: 3mm or 1/4 inch).\n\n");
        break;
    
    case 11:
        fprintf(file, "ERROR 11: Ups! It looks like the tool diameter unit isn't valid (ex.: either mm or iNc3hes are valid but 3hfu349t9 isn't).\n\n");
        break;
    
    case 12:
        fprintf(file, "ERROR 12: Ups! It looks like the material isn't supported (ex.: either Wood or WoO0ds are valid/supported but 3hfu349t9 isn't and Unobtanium isn't supported).\n\n");
        break;
    
    case 13:
        fprintf(file, "ERROR 13: Ups! For that material the ChiploadTable.csv lacks data to satisfy the tool you want to use.\n\n");
        break;

    default:
        fprintf(file, "ERROR DEFAULT: UNDOCUMENTED RANDOM ERROR :(\n\n");
        break;
    }

    fprintf(file, "==================================\n\n\n"); // enclose error message to user

    // Close the file
    fclose(file);
}


/**
 * WarningMessage: writes a warning message to a file based on the error code provided.
 * 
 * Parameters:
 * @param filename: The name of the file to write the warning message to.
 * @param warning: The warning code to determine the specific warning message to write.
 * 
 * Returns:
 * @return This function does not return a value.
 */
void WarningMessage(const char *filename, int warning) {
    FILE *file = fopen(filename, "a");  // open file in append mode
    if (file == NULL) {                 // handles case where file can't be accessed
        printf("Error opening file %s\n", filename); 
        return;
    }

    if ((number_warnings = 0))          // print warning header if it's the first error
        fprintf(file, "============= WARNINGS =============\n");

    ++number_warnings;                  // for counting errors
   
    switch (warning)                    // print error message based on error
    {
    case 6:
        fprintf(file, "Warning 6: You didn't specify the units you want the results to be displayed, the feedrate was calculated in mm/m.\n\n");
        break;
    
    case 7:
        fprintf(file, "Warning 7: You didn't specify how many cutting edges your tool has, the calculation has resumed with 2 cutting edges\n as it is the most common type. Make sure the tool has 2 cutting edges before resuming with any machining!\n\n");
        break;

    case 9:
        fprintf(file, "Warning 9: You didn't specify the job quality, the values were calculated with the default of 3 (balanced).\n\n");
        break;
    
    case 10:
        fprintf(file, "Warning 10: You didn't enter a valid job quality (finish 1 - 5 speed), the values were calculated with the default of 3 (balanced).\n\n");
        break;
    
    case 14:
        fprintf(file, "Warning 14: You didn't specify the units you want the results to be displayed, the feedrate was calculated in mm/m.\n\n");
        break;
    
    case 15:
        fprintf(file, "Warning 15: BE CAREFUL!!! The feed is to high for the machine. You should get a tool with less cutting edges, smaller diameter or even both.\n");
        fprintf(file, "Still if you know what you are doing you could try to run the machine at its minimum feed for its maximum feedrate of %d mm/m @%d rpm\n\n", CNCMAXFEED, CNCMINSPEED);
        break;

    default:
        fprintf(file, "WARNING DEFAULT: UNKOWN WARNING :(\n\n");
        break;
    }

    // Close the file
    fclose(file);
}


/**
 * WriteResultstoFile: writes the calculation results to a file, including details about the tool, material, speed, and generated results.
 * 
 * Parameters:
 * Parameters:
 * @param filename: The name of the file to write the results to.
 * @param material: The material being cut.
 * @param tool_diameter: The diameter of the cutting tool.
 * @param tool_unit: The unit of the tool diameter.
 * @param tool_teeth: The number of teeth on the tool.
 * @param speed: The speed/quality index for the job.
 * @param results: The calculated feedrate and RPM.
 * @param feed_rate: The feed rate value.
 * @param out_unit: The unit for the output feed rate.
 * @param materials_list: Array containing the list of materials.
 * @param checklist: A flag indicating whether to include a checklist in the results.
 * @param supported_materials_list: A flag indicating whether to include a list of supported materials.
 * 
 * Returns:
 * @return true if the results were successfully written to the file, false otherwise.
 */
bool WriteResultsToFile(const char *filename, const char *material, float tool_diameter, const char*tool_unit, int tool_teeth, float speed, Point results, const float feed_rate, char *out_unit, char *materials_list[], bool checklist, bool supported_materials_list) {
    FILE *file = fopen(filename, "a");      // open file in append mode
    if (file == NULL) {                     // handles case where file can't be accessed
        printf("Error opening file %s\n", filename); 
        return false;
    }

    // Initialize variable to print depth of cut
    float depth_of_cut = tool_diameter / 2;

    // TODO: write logic to display depth of cut in inches if the tool is in inches

    // Write the results to the file
    fprintf(file, "\n\n====================================================================================\n");
    fprintf(file, "                      NEW TOOL: %.2f %s (%i flutes) for %s\n", tool_diameter, tool_unit, tool_teeth, material);
    fprintf(file, "====================================================================================\n\n");
    fprintf(file, "Parameters optimized for quality/speed value of %1.f:\n", speed);
    fprintf(file, "Feedrate: %.1f %s\n", feed_rate, out_unit);
    fprintf(file, "RPM:      %i rpm\n\n", results.x);
    fprintf(file, "Remember that this is a good starting point, first you should try testing it in a\n");
    fprintf(file, "small piece of %s and note how it goes. Adjust it as needed or try to get\n", material);
    fprintf(file, "different values by changing the job speed/finish (or other parameters). When testing\n");
    // TODO: write logic depth of cut in metals and very hard or gummy materials
    fprintf(file, "start with a relatively low depth of cut of %.2f %s and increment it until a max of\n", depth_of_cut, tool_unit);
    fprintf(file, "%.2f %s. If dealing with metals like aluminum or steel don't go above %.2f %s.\n\n\n", tool_diameter, tool_unit, depth_of_cut, tool_unit);
    // Print checklist if user specifies it
    if (checklist) {
        fprintf(file, "=========\n");
        fprintf(file, "CHECKLIST\n");
        fprintf(file, "=========\n\n");
        fprintf(file, "□ Go over CAD model and check dimensions.\n");
        fprintf(file, "□ Go over CAD model and check what is the smallest path width in the design (should be equal or more than the tool diameter being used).\n");
        fprintf(file, "□ Go over the tool paths and check if all the parameters are correct.\n");
        fprintf(file, "□ Does the reference point and stock material in CAD correctly match the machine setup?\n");
        fprintf(file, "□ Is the stock material firmly secured in place?\n");
        fprintf(file, "□ Is any of the fixing hardware in the way of the toolpath?.\n");
        fprintf(file, "□ Is the CNC currectly homed?\n");
        fprintf(file, "□ Is the CNC tool currectly fixed?\n");
        fprintf(file, "□ Is the CNC tool lenght measured?\n");
        fprintf(file, "□ Is the CNC zero point currectly setup matching the CAD reference point for the toolpaths?\n");
        fprintf(file, "□ Observe from a safe place, if possible, the machine running, take notes of what you see\n");
        fprintf(file, "□ Observe the machined piece, take notes\n");
        fprintf(file, "□ If you observed something out of the ordinary or the results where insatisfactory, collect your notes, Search for possible solutions and/or ask for help\n\n\n");
    }

    // Print materials list if user specifies it
    if (supported_materials_list) {
        fprintf(file, "===================\n");
        fprintf(file, "%u Materials Supported:\n", unique_materials_count);
        fprintf(file, "===================\n\n");

        for (unsigned int i = 0; i < unique_materials_count; i++) {
            fprintf(file, "%s\n", materials_list[i]);
        }
        
        fprintf(file, "\n");
    }

    fprintf(file, "=======================================================================================\n\n\n");
    
    // Close the file
    fclose(file);

    // Return
    return true;
}