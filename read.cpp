#include <iostream>     // for standard C++ library input and output
#include <fstream>      // for file stream operations
#include <string>       // for std::string
#include <algorithm>    // for std::remove and std::find
#include "chipload.h"   // for external user defined functions

// Function to trim newline characters from a std::string
void TrimNewline(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
}

// Function to read data from a file and populate the variables
bool ReadFromFile(const std::string &filename, bool &beginner, std::string &material, std::string &tool_diam, std::string &tool_z, std::string &job_quality, std::string &out_units, bool &checklist, bool &supported_materials_list) {
    std::ifstream file(filename); // Open file in read mode
    if (!file.is_open()) {        // Handle case where file can't be accessed
        std::cerr << "Error opening file" << std::endl;
        return false;
    }

    std::string line;

    while (std::getline(file, line)) { // Read file line by line until EOF
        TrimNewline(line); // Trim newline characters from line

        if (line.rfind("I'm a beginner:", 0) == 0) {
            beginner = (line.find('y') != std::string::npos || line.find('Y') != std::string::npos);
        } else if (line.rfind("Material to cut:", 0) == 0) {
            material = line.substr(16); // Extract material from line
        } else if (line.rfind("Tool Diameter:", 0) == 0) {
            tool_diam = line.substr(14); // Extract tool diameter from line
        } else if (line.rfind("Tool Flutes:", 0) == 0) {
            tool_z = line.substr(12); // Extract tool flutes from line
        } else if (line.rfind("Job Quality:", 0) == 0) {
            job_quality = line.substr(12); // Extract job quality from line
        } else if (line.rfind("I want to get the FeedRate in:", 0) == 0) {
            out_units = line.substr(30); // Extract output units from line
        } else if (line.rfind("Print a generic CNC CHECKLIST for the job:", 0) == 0) {
            checklist = (line.find('y') != std::string::npos || line.find('Y') != std::string::npos);
        } else if (line.rfind("Print a LIST of supported materials:", 0) == 0) {
            supported_materials_list = (line.find('y') != std::string::npos || line.find('Y') != std::string::npos);
        }
    }

    file.close(); // Close file
    return true;  // Return success
}
