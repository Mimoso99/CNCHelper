#include <iostream>     // for standard C++ library for input and output
#include <cctype>       // for character handling functions
#include <cstdio>       // for standard input/output operations
#include <cstdlib>      // for memory allocation
#include <cstring>      // for string manipulation functions
#include <string>       // for std::string
#include "chipload.h"   // for external user defined functions

// Hash table
Node* table[N_BUCKETS];

// Variables for future use
unsigned int material_count = 0;         // positive integer counter for the number of unique materials loaded into memory
unsigned int unique_materials_count = 0; // positive integer counter for the number of unique materials loaded into memory

/**
 * Calculate the Hash value for a given word using the djb2 Hash function.
 * 
 * @param word The word for which the Hash value needs to be calculated.
 * @return The Hash value of the word.
 */
unsigned int Hash(const std::string &word)
{
    unsigned long hash_value = 5381;
    for (char c : word)
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
bool Load(const std::string& filename)
{
    FILE *file = fopen(filename.c_str(), "r");
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
        std::string str_line(line); // Convert C-string to std::string
        size_t comma1 = str_line.find(',');
        if (comma1 == std::string::npos) continue; // Skip line if no comma

        std::string material = str_line.substr(0, comma1);
        str_line = str_line.substr(comma1 + 1);
        
        float diameter, chipload, factor;
        int scanned = sscanf(str_line.c_str(), "%f, %f, %f", &diameter, &chipload, &factor);
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
        Node *cursor = table[index];
        bool unique = true;
        while (cursor != NULL)
        {
            if (strcasecmp(cursor->material.c_str(), material.c_str()) == 0 && cursor->diameter == diameter) {
                unique = false;
                break;
            }
            cursor = cursor->next;
        }
        
        if (!unique) {
            continue; // Skip if material and diameter are already present
        }

        // Create a new node for the material
        Node *n = (Node *)malloc(sizeof(Node));
        if (!n)
        {
            printf("Failed to allocate memory for a new node (LookupTable chipload data structure)");
            fclose(file);
            return false;
        }
        n->material = material;
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
bool Search(const std::string &material, float diameter, float &chipload, float &rpm_factor)
{
    unsigned int index = Hash(material);
    Node *cursor = table[index];
    while (cursor != NULL)
    {
        if (strcasecmp(cursor->material.c_str(), material.c_str()) == 0 && cursor->diameter == diameter)
        {
            printf("Found: Material=%s, Diameter=%.2f, Chipload=%.2f, Factor=%.2f\n",
                   cursor->material.c_str(), cursor->diameter, cursor->chipload, cursor->factor);
            chipload = cursor->chipload;
            rpm_factor = cursor->factor;
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
        Node* cursor = table[i];
        while (cursor)
        {
            Node* tmp = cursor;
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
        Node *cursor = table[i];
        if (cursor != NULL)
        {
            printf("Bucket %u:\n", i);
            while (cursor != NULL)
            {
                printf("  Material: %s, Diameter: %.2f, Chipload: %.2f, Factor: %.2f\n",
                       cursor->material.c_str(), cursor->diameter, cursor->chipload, cursor->factor);
                cursor = cursor->next;
            }
        }
    }
}
