/**
 * This file contains the following function definitions for reading user input:
 * - TrimNewline
 * - ReadFromFile
 */

// Include headers & libraries (<stdbool.h> is included in the header "chipload.h")
#include <iostream>     // for standard C++ library for input and output
#include <cctype>       // for character handling functions
#include <cstdio>       // for standard input/output operations
#include <cstdlib>      // for memory allocation
#include <cstring>      // for string manipulation functions
#include "chipload.h"   // for external user defined functions


/**
 * TrimNewLine: trims newline characters from a string.
 * 
 * Parameters:
 * @param str The string to trim newline characters from.
 * 
 *  Returns:
 * @return This function does not return a value.
 */
void TrimNewline(char *string) {
    char *position;
    if ((position = strchr(string, '\n')) != NULL)
        *position = '\0';
    if ((position = strchr(string, '\r')) != NULL)
        *position = '\0';
}


/**
 * ReadFromFile: reads data line by line from a text file and populates the provided variables
 *               based on the content of each line.
 * 
 * Parameters:
 * @param filename The name of the file to read from.
 * @param beginner Pointer to a boolean variable to store if the user is a beginner.
 * @param material Pointer to a string variable to store the material to cut.
 * @param tool_diam Pointer to a string variable to store the tool diameter.
 * @param tool_z Pointer to a string variable to store the tool flutes.
 * @param job_quality Pointer to a string variable to store the job quality.
 * @param out_units Pointer to a string variable to store the desired output units.
 * @param checklist Pointer to a boolean variable to store if a checklist is requested for the job.
 * @param supported_materials_list Pointer to a boolean variable to store if a list of supported materials is requested.
 * 
 * Returns:
 * @return true if the file was successfully read and variables were populated, false otherwise.
 */
bool ReadFromFile(const char *filename, bool *beginner, char **material, char **tool_diam, char **tool_z, char **job_quality, char **out_units, bool *checklist, bool *supported_materials_list) {
    FILE *file = fopen(filename, "r");  // open file in read mode
    if (file == NULL) {                 // handles case where file can't be accessed
        printf("Error opening file\n");
        return false;
    }

    char line[MAX_LINE_LENGTH]; // line buffer of size MAX_LINE_LENGTH

    while (fgets(line, sizeof(line), file)) { // read file line by line (storing the line in 'line') until EOF
        
        TrimNewline(line); // trim newline characters from line (easier handling of user input)
        
        if (strncmp(line, "I'm a begginer:", 15) == 0) {        // if it encounters that specific character sequence in line,
            if (strchr(line, 'y') || strchr(line, 'Y')) {       // searches for user input 'y' or 'Y'
            *beginner = true;                                   // sets flag to true
            }
        }
        else if (strncmp(line, "Material to cut:", 16) == 0) {  // if it encounters that specific character sequence in line,
            // free(*material);                                 // frees previously allocated memory, all these pointers are initialized to NULL making this line not needed
            *material = (char *)malloc(strlen(line + 16) + 1);  // alocate enough space for material
            if (*material == NULL) {                            // handle failed allocation
                printf("Memory allocation failed for the material\n");
                return false;
            }
            strcpy(*material, line + 16);                       // copy line from character 16 into material
        }
        else if (strncmp(line, "Tool Diameter:", 14) == 0) {
            *tool_diam = (char *)malloc(strlen(line + 14) + 1);
            if (*tool_diam == NULL) {
                printf("Memory allocation failed for the tool diameter\n");
                return false;
            }
            strcpy(*tool_diam, line + 14);
        } 
        else if (strncmp(line, "Tool Flutes:", 12) == 0) {
            *tool_z = (char *)malloc(strlen(line + 12) + 1);
            if (*tool_z == NULL) {
                printf("Memory allocation failed for the tool flutes\n");
                return false;
            }
            strcpy(*tool_z, line + 12);
        } 
        else if (strncmp(line, "Job Quality:", 12) == 0) {
            *job_quality = (char *)malloc(strlen(line + 12) + 1);
            if (*job_quality == NULL) {
                printf("Memory allocation failed for the quality\n");
                return false;
            }
            strcpy(*job_quality, line + 12);
        }
        else if (strncmp(line, "I want to get the FeedRate in:", 30) == 0) {
            *out_units = (char *)malloc(strlen(line + 30) + 1);
            if (*out_units == NULL) {
                printf("Memory allocation failed for the out unit\n");
                return false;
            }
            strcpy(*out_units, line + 30);
        }
        else if (strncmp(line, "Print a generic CNC CHECKLIST for the job:", 42) == 0) {
            if (strchr(line, 'y') || strchr(line, 'Y')) {
            *checklist = true;
            }
        }
        else if (strncmp(line, "Print a LIST of supported materials:", 36) == 0) {
            if (strchr(line, 'y') || strchr(line, 'Y')) {
            *supported_materials_list = true;
            }
        }
    }

    // Close file
    fclose(file);

    // Return happy path
    return true;
}