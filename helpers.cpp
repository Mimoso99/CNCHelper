#include <iostream>     // for standard C++ library for input and output
#include <cctype>       // for character handling functions
#include <cstdio>       // for standard input/output operations
#include <cstdlib>      // for memory allocation
#include <cstring>      // for string manipulation functions
#include <string>       // for std::string
#include "chipload.h"   // for external user defined functions

// external variable
extern Node *table[N_BUCKETS]; // Ensure this is declared somewhere in your code

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
bool UniqueElements(std::vector<std::string> &unique_materials, unsigned int *material_counter) {
    unique_materials.clear(); // Clear the vector to start fresh
    *material_counter = 0;

    for (unsigned int i = 0; i < N_BUCKETS; i++) {
        Node *cursor = table[i];
        while (cursor != NULL) {
            bool unique = true;
            // check if the material is already in the unique_materials vector
            for (unsigned int j = 0; j < unique_materials.size(); j++) {
                if (strcasecmp(cursor->material.c_str(), unique_materials[j].c_str()) == 0) {
                    unique = false;
                    break;
                }
            }
            // If the material is unique, add it to the unique_materials vector
            if (unique) {
                unique_materials.push_back(cursor->material);
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
void CleanString(std::string &source) {
    std::string result;
    for (char c : source) {
        if (isalpha(static_cast<unsigned char>(c))) {
            result += c;
        }
    }
    source = result;
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
float CleanNumber(const std::string &source) {
    std::string cleanedStr;
    int index = 0;
    bool has_decimal_point = false;
    bool has_fraction = false;
    int numerator = 0;
    int denominator = 0;

    for (char c : source) {
        if (isdigit(static_cast<unsigned char>(c))) {
            cleanedStr += c;
            if (has_fraction) {
                denominator = denominator * 10 + (c - '0');
            } else {
                numerator = numerator * 10 + (c - '0');
            }
        } else if (c == '.' && !has_decimal_point) {
            cleanedStr += c;
            has_decimal_point = true;
        } else if (c == '/') {
            has_fraction = true;
            cleanedStr.clear(); // Clear cleanedStr before processing the denominator
            numerator = std::stoi(cleanedStr); // Convert the numerator part
        }
    }

    if (has_fraction && denominator != 0) {
        return static_cast<float>(numerator) / denominator;
    } else {
        return std::stof(cleanedStr); // Convert the cleaned string to float
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
int LevenshteinDistance(const std::string &string1, const std::string &string2) {
    int len1 = string1.length();
    int len2 = string2.length();
    int **d = (int **)malloc((len1 + 1) * sizeof(int *));
    for (int i = 0; i <= len1; i++) {
        d[i] = (int *)malloc((len2 + 1) * sizeof(int));
    }

    for (int i = 0; i <= len1; i++) d[i][0] = i;
    for (int j = 0; j <= len2; j++) d[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (tolower(static_cast<unsigned char>(string1[i - 1])) == tolower(static_cast<unsigned char>(string2[j - 1]))) ? 0 : 1;
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
 * @param dictionary: A vector of units to compare against.
 * @param max_distance: The maximum allowed Levenshtein distance for a valid match.
 * 
 * Returns:
 * @return the best matching unit from the vector based on the Levenshtein distance, or "error" if no match is found.
 */
std::string BestMatch(const std::string &source, const std::vector<std::string> &dictionary, int max_distance) {
    int min_distance = LevenshteinDistance(source, dictionary[0]);
    std::string best_match = dictionary[0];

    for (size_t i = 1; i < dictionary.size(); i++) {
        int distance = LevenshteinDistance(source, dictionary[i]);
        if (distance < min_distance && distance <= max_distance) {
            min_distance = distance;
            best_match = dictionary[i];
        }
    }
    if (min_distance > max_distance) {
        return "error";
    }
    
    return best_match;
}

// struct with units and a multiplier that compares to mm/s
struct conversion {
    std::string unit;
    float multiplier;
} conversions[] = {
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
 * @param to: The unit to convert the value to.
 * 
 * Returns:
 * @return The converted value after applying the appropriate conversion factor.
 * @return Returns -1 if either the 'from' or 'to' unit is not found in the predefined conversion table.
 */
float Convert(float value, const std::string &from, const std::string &to)
{
    int size = sizeof(conversions) / sizeof(conversions[0]);
    float from_multiplier = -1;
    float to_multiplier = -1;

    for (int i = 0; i < size; ++i) {
        if (conversions[i].unit == from) {
            from_multiplier = conversions[i].multiplier;
            break;
        }
    }

    for (int j = 0; j < size; ++j) {
        if (conversions[j].unit == to) {
            to_multiplier = conversions[j].multiplier;
            break;
        }
    }

    if (from_multiplier == -1 || to_multiplier == -1) {
        return -1;
    } else {
        return value * (from_multiplier / to_multiplier);
    }
}
