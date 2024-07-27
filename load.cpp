/**
 * This file contains the following function definitions for loading material data from .csv,
 * and helpful function to Search and operato trhough said data:
 * - Hash
 * - Load
 * - Search
 * - size
 * - Unload
 * - print table
 */

// Include headers & libraries (<stdbool.h> is included in the header "chipload.h")
#include <iostream>     // for standard C++ library for input and output
#include <cctype>       // for character handling functions
#include <cstdio>       // for standard input/output operations
#include <cstdlib>      // for memory allocation
#include <cstring>      // for string manipulation functions
#include "chipload.h"   // for external user defined functions


// Hash table
node* table[N_BUCKETS];

// Variables for future use
unsigned int material_count = 0;         // positive integer counter for the number of unique materials loaded into memory
unsigned int unique_materials_count = 0; // positive integer counter for the number of unique materials loaded into memory


/**
 * Calculate the Hash value for a given word using the djb2 Hash function.
 * 
 * @param word The word for which the Hash value needs to be calculated.
 * @return The Hash value of the word.
 */
unsigned int Hash(const char *word)
{
    unsigned long hash_value = 5381;
    int c;
    while ((c = *word++))
    {
        hash_value = ((hash_value << 5) + hash_value) + toupper(c); // hash_value * 33 + c
    }
    return hash_value % N_BUCKETS;
}


/**
 * Load material data from a CSV file into a Hash table.
 * 
 * @param filename The name of the CSV file to Load data from.
 * @return true if the data is loaded successfully, false otherwise.
 */
bool Load(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Can't open file\n");
        return false;
    }

    // Initialize the Hash table
    for (unsigned int i = 0; i < N_BUCKETS; i++)
    {
        table[i] = NULL;
    }

    char line[MAX_LINE_LENGTH];
    if (fgets(line, sizeof(line), file) == NULL) // Skip the header line
    {
        fclose(file);
        return false;
    }

    while (fgets(line, sizeof(line), file))
    {
        char material[MAX_WORD_LENGTH + 1];
        float diameter, chipload, factor;
        int scanned = sscanf(line, "%45[^,], %f, %f, %f", material, &diameter, &chipload, &factor);
        if (scanned < 3) // Skip line if parsing fails
        {
            continue;
        }
        if (scanned == 3)
        {
            factor = 1.0; // Default factor if not provided
        }

        // Check for uniqueness
        unsigned int index = Hash(material);
        node *cursor = table[index];
        bool unique = true;
        while (cursor != NULL)
        {
            if (strcasecmp(cursor->material, material) == 0 && cursor->diameter == diameter) {
                unique = false;
                break;
            }
            cursor = cursor->next;
        }
        
        if (!unique) {
            continue; // Skip if material and diameter are already present
        }

        // Create a new node for the material
        node *n = malloc(sizeof(node));
        if (!n)
        {
            printf("Failed to allocate memory for a new node (LookupTable chipload data structure)");
            fclose(file);
            return false;
        }
        strcpy(n->material, material);
        n->diameter = diameter;
        n->chipload = chipload;
        n->factor = factor;
        n->next = table[index];
        table[index] = n;
        material_count++;
    }

    fclose(file);
    return true;
}


/**
 * Search for a material and diameter in the Hash table.
 * 
 * @param material The material to Search for.
 * @param diameter The diameter to Search for.
 * @param chipload Pointer to store the found chipload value.
 * @param rpm_factor Pointer to store the found RPM factor value.
 * @return true if the material and diameter are found in the Hash table, false otherwise.
 */
bool Search(const char *material, float diameter, float *chipload, float *rpm_factor)
{
    unsigned int index = Hash(material);
    node *cursor = table[index];
    while (cursor != NULL)
    {
        if (strcasecmp(cursor->material, material) == 0 && cursor->diameter == diameter)
        {
            printf("Found: Material=%s, Diameter=%.2f, Chipload=%.2f, Factor=%.2f\n",
                   cursor->material, cursor->diameter, cursor->chipload, cursor->factor);
            *chipload = cursor->chipload;
            *rpm_factor = cursor->factor;
            return true;
        }
        cursor = cursor->next;
    }
    return false;
}

// Function to return the number of words in the dictionary
unsigned int size(void)
{
    return material_count;
}


/**
 * Unload the Hash table from memory by freeing all allocated nodes.
 * 
 * @return true if the Hash table is successfully unloaded, false otherwise.
 */
bool Unload(void)
{
    for (int i = 0; i < N_BUCKETS; i++)
    {
        node *cursor = table[i];
        while (cursor)
        {
            node *tmp = cursor;
            cursor = cursor->next;
            free(tmp);
        }
    }
    return true;
}


/**
 * Print the contents of the Hash table, including material, diameter, chipload, and factor for each entry.
 */
void PrintTable(void)
{
    for (unsigned int i = 0; i < N_BUCKETS; i++)
    {
        node *cursor = table[i];
        if (cursor != NULL)
        {
            printf("Bucket %u:\n", i);
            while (cursor != NULL)
            {
                printf("  Material: %s, Diameter: %.2f, Chipload: %.2f, Factor: %.2f\n",
                       cursor->material, cursor->diameter, cursor->chipload, cursor->factor);
                cursor = cursor->next;
            }
        }
    }
}