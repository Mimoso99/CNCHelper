// Chipload.h
#ifndef CHIPLOAD_H
#define CHIPLOAD_H

// Header files
#include <iostream>     // for standard C++ library for input and output
#include <string>      // for std::string
#include <vector>      // for std::vector

// Constant Expressions for CNC LIMITS
#define CNCPOWER 3000       // CNC max power in watts
#define CNCMAXFEED 4080     // CNC max feedrate in mm/m
#define MIN_Y 0             // CNC minimum feedrate, it's zero but could be later incorpored in the Simplex and Midpoint function as CNCMINFEED
#define CNCMINSPEED 10000   // CNC min rotational speed in rpm
#define CNCMAXSPEED 24000   // CNC max rotational speed in rpm
#define MAXDEV 0.01         // Max deviation allowed for chipload

// Constant Expressions
#define MAX_LINE_LENGTH 256 //defines max string lenght for reading
#define MAX_WORD_LENGTH 45  // max word length
#define N_BUCKETS 26        // Number of buckets in Hash table

// Represents a node in a Hash table
struct Node {
    std::string material;  // Changed from char[] to std::string
    float diameter;
    float chipload;
    float factor;
    Node* next;
};

// Represents a point with x and y coordinates
struct Point {
    int x;
    int y;
};

// Declaration of external variables
extern Node* table[N_BUCKETS];          // Changed from array to vector
extern unsigned int unique_materials;  // Changed from array to vector
extern unsigned int unique_materials_count;


// Function Prototypes
bool UniqueElements(std::vector<std::string>& unique_materials, unsigned int& material_counter);
bool ReadFromFile(const std::string& filename, bool& beginner, std::string& material, std::string& tool_diam, std::string& tool_z, std::string& job_quality, std::string& out_units, bool& checklist, bool& supported_materials_list);
void CleanString(std::string& source);
float CleanNumber(const std::string& source);
std::string BestMatch(const std::string& source, const std::vector<std::string>& dictionary, int max_distance);
bool Load(const std::string& filename);
bool Search(const std::string& material, float diameter, float& chipload, float& rpm_factor);
bool Unload();
void PrintTable();
Point Simplex(int x_min, int x_max, int y_max, float a, float b, bool maximize_y);
Point Midpoint(int x_min, int x_max, int y_max, float c);
bool WriteResultsToFile(const std::string& filename, const std::string& material, float tool_diameter, const std::string& tool_unit, int tool_teeth, float speed, Point results, float feed_rate, const std::string& out_unit, const std::vector<std::string>& materials_list, bool checklist, bool supported_materials_list);
float Convert(float value, const std::string& from, const std::string& to);
void ErrorMessage(const std::string& filename, int error);
void WarningMessage(const std::string& filename, int warning);

#endif