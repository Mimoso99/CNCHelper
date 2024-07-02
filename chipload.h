// Chipload.h
#ifndef CHIPLOAD_H
#define CHIPLOAD_H

// Header files
#include <stdbool.h>

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
typedef struct node
{
    char material[MAX_WORD_LENGTH + 1];
    float diameter;
    float chipload;
    float factor;
    struct node *next;
} node;

// Represents a point with x and y coordinates
typedef struct {
    int x;
    int y;
} Point;

// Declaration of external variables
extern node *table[N_BUCKETS];
extern char *unique_materials[N_BUCKETS];
extern unsigned int unique_materials_count;


// Function Prototypes
bool UniqueElements(char *unique_materials[], unsigned int *material_counter);
bool ReadFromFile(const char *filename, bool *beginner, char **material, char **tool_diam, char **tool_z, char **job_quality, char **out_units, bool *checklist, bool *supported_materials_list);
void CleanString(char *source);
float CleanNumber(const char *source);
char* BestMatch(char *source, char *dictionary[], int dictionary_size, int max_distance);
bool Load(const char *filename);
bool Search(const char *material, float diameter, float *chipload, float *rpm_factor);
bool Unload(void);
void PrintTable(void);
Point Simplex(int x_min, int x_max, int y_max, float a, float b, bool maximize_y);
Point Midpoint(int x_min, int x_max, int y_max, float c);
bool WriteResultsToFile(const char *filename, const char *material, float tool_diameter, const char *tool_unit, int tool_teeth, float speed, Point results, const float feed_rate, char *out_unit, char *materials_list[], bool checklist, bool supported_materials_list);
float Convert(float value, char* from, char* to);
void ErrorMessage(const char *filename, int error);
void WarningMessage(const char *filename, int warning);

#endif