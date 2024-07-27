/**
 * This file contains the following function definitions for several helpful functions to main:
 * - UniqueElements
 * - CleanString
 * - CleanNumber
 * - min
 * - LevenshteinDistance
 * - BestMatch
 * - Convert
 */

// Include headers & libraries (<stdbool.h> is included in the header "chipload.h")
#include <ctype.h>      // for character handling functions
#include <stdio.h>      // for standard input/output operations
#include <stdlib.h>     // for memory allocation
#include <string.h>     // for string manipulation functions
#include <strings.h>    // for more string manipulation functions
#include "chipload.h"   // for external user defined functions


// external variable
extern node *table[N_BUCKETS]; // Ensure this is declared somewhere in your code


/**
 * Function: initializes unique materials by checking and adding them to the unique_materials array.
 * 
 * Parameters:
 * @param unique_materials: An array of unique materials to be initialized.
 * @param material_counter: A pointer to the count of unique materials.
 * 
 * Returns:
 * @return true if the initialization is successful, false otherwise.
 */
bool UniqueElements(char *unique_materials[], unsigned int *material_counter) {
    for (unsigned int i = 0; i < N_BUCKETS; i++) {
        node *cursor = table[i];
        while (cursor != NULL) {
            bool unique = true;
            // check if the material is already in the unique_materials array
            for (unsigned int j = 0; j < *material_counter; j++) {
                if (strcasecmp(cursor->material, unique_materials[j]) == 0) {
                    unique = false;
                    break;
                }
            }
            // If the material is unique, add it to the unique_materials array
            if (unique) {
                unique_materials[*material_counter] = malloc(strlen(cursor->material) + 1);
                if (unique_materials[*material_counter] == NULL) {
                    return false;
                }
                strcpy(unique_materials[*material_counter], cursor->material);
                (*material_counter)++;
            }
            cursor = cursor->next;
        }
    }
    return true;
}


/**
 * Function: cleans a string by retaining only alphabetical characters.
 * 
 * Parameters:
 * @param source: The input string to be cleaned.
 * 
 * Description:
 * This function iterates through the input string and copies only the alphabetical characters to the destination array.
 * It null-terminates the cleaned string at the end.
 */
void CleanString(char *source) {
    char *dest = source; // Pointer to track the destination in the same array

    while (*source) {
        if (isalpha((unsigned char)*source)) {
            *dest = *source; // Copy alphabet character to destination
            dest++; // Move destination pointer forward
        }
        source++; // Move source pointer forward
    }
    *dest = '\0'; // Null-terminate the cleaned string
}


/**
 * Function: cleans the input string by extracting and processing numbers, including fractions and floating-point numbers.
 * 
 * Parameters:
 * @param source: The input string containing the number to be cleaned.
 * 
 * Returns:
 * @return The cleaned number as a float after processing any fractions or decimal points present in the input string.
 */
float CleanNumber(const char *source) {
    char cleanedStr[100]; // Large enough buffer to hold the cleaned number string
    int index = 0;
    int has_decimal_point = 0;
    int has_fraction = 0;
    int numerator = 0;
    int denominator = 0;

    while (*source) {
        if (isdigit((unsigned char)*source)) {
            cleanedStr[index++] = *source;
            if (has_fraction) {
                denominator = denominator * 10 + (*source - '0');
            } else {
                numerator = numerator * 10 + (*source - '0');
            }
        } else if (*source == '.' && !has_decimal_point) {
            cleanedStr[index++] = *source;
            has_decimal_point = 1;
        } else if (*source == '/') {
            has_fraction = 1;
            cleanedStr[index] = '\0'; // Null-terminate before processing the denominator
            numerator = atoi(cleanedStr); // Convert the numerator part
            index = 0; // Reset index for the denominator
        }
        source++;
    }

    if (has_fraction && denominator != 0) {
        return (float)numerator / denominator;
    } else {
        cleanedStr[index] = '\0'; // Null-terminate the cleaned string
        return atof(cleanedStr); // Convert the cleaned string to float
    }
}


/**
 * Returns the minimum value among three integers.
 */
int min(int a, int b, int c) {
    if (a < b && a < c) return a;
    if (b < c) return b;
    return c;
}


/**
 * Function: calculates the Levenshtein distance between two input strings.
 * 
 * Parameters:
 * @param string1: The first input string.
 * @param string2: The second input string.
 * 
 * Returns:
 * @return The Levenshtein distance between the two input strings.
 */
int LevenshteinDistance(const char *string1, const char *string2) {
    int len1 = strlen(string1);
    int len2 = strlen(string2);
    int **d = (int **)malloc((len1 + 1) * sizeof(int *));
    for (int i = 0; i <= len1; i++) {
        d[i] = (int *)malloc((len2 + 1) * sizeof(int));
    }

    for (int i = 0; i <= len1; i++) d[i][0] = i;
    for (int j = 0; j <= len2; j++) d[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (tolower(string1[i - 1]) == tolower(string2[j - 1])) ? 0 : 1;
            d[i][j] = min(
                d[i - 1][j] + 1,    // Deletion
                d[i][j - 1] + 1,    // Insertion
                d[i - 1][j - 1] + cost // Substitution
            );
        }
    }

    int distance = d[len1][len2];

    for (int i = 0; i <= len1; i++) {
        free(d[i]);
    }
    free(d);

    return distance;
}


/**
 * Function: finds the best match from a list of units based on the Levenshtein distance.
 * 
 * Parameters:
 * @param source: The input unit to find the best match for.
 * @param dictionary: An array of units to compare against.
 * @param dictionary_size: The size of the units array.
 * @param max_distance: The maximum allowed Levenshtein distance for a valid match.
 * 
 * Returns:
 * @return the best matching unit from the array based on the Levenshtein distance, or "error" if no match is found.
 */
char* BestMatch(char *source, char *dictionary[], int dictionary_size, int max_distance) {
    int min_distance = LevenshteinDistance(source, dictionary[0]);
    char *best_match = dictionary[0];

    for (int i = 1; i < dictionary_size; i++) {
        int distance = LevenshteinDistance(source, dictionary[i]);
        if (distance < min_distance && distance <= max_distance) {
            min_distance = distance;
            best_match = dictionary[i];
        }
    }
    if (best_match == NULL) {
        best_match = "error";
    }
    
    return best_match;
}


// struct with units and a multiplier that compares to mm/s
struct convertion {
    char *unit;
    float multiplier;
} convertions[] = {
    {"mm", (1.0 / 1.0)},
    {"in", (25.4 / 1.0)},
    {"inch", (25.4 / 1.0)},
    {"inches", (25.4 / 1.0)},
    {"mm/s", (1.0 / 1.0)},
    {"mm/m", (1.0 / 60.0)},
    {"m/m", (1000.0 / 60.0)},
    {"in/s", (25.4 / 1.0)},
    {"in/m", (25.4 / 60.0)},
    {"inch/s", (25.4 / 1.0)},
    {"inch/m", (25.4 / 60.0)},
    {"feet/m", (304.8 / 60.0)},
};


/**
 * Function: converts a given value from one unit to another based on predefined conversion factors.
 * 
 * Parameters:
 * @param value: The numerical value to be converted.
 * @param from: The unit of the input value.
 * @param to: The unit to Convert the value to.
 * 
 * Returns:
 * @return The converted value after applying the appropriate conversion factor.
 * @return Returns -1 if either the 'from' or 'to' unit is not found in the predefined conversion table.
 */
float Convert(float value, char* from, char* to)
{   
    int size = sizeof(convertions) / sizeof(convertions[0]);
    float from_multiplier = -1;
    float to_multiplier = -1;

    for (int i = 0; i < size; ++i) {
        if (strcmp(convertions[i].unit, from) == 0) {
            from_multiplier = convertions[i].multiplier;
            break;
        }
    }

    for (int j = 0; j < size; ++j) {
        if (strcmp(convertions[j].unit, to) == 0) {
            to_multiplier = convertions[j].multiplier;
            break;
        }
    }

    if (from_multiplier == -1 || to_multiplier == -1) {
        return -1;
    }

    else {
    return value * (from_multiplier / to_multiplier);
    }
}