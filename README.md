# CNCHelper
### Video Demo:  [YouTube Link](https://youtu.be/gBs3X3mtbmg)
### Description:
**CNChelper** (the name needs work), is a calculator app in C that abstracts several steps and considerations when calculating a suitable feedrate and speed for a CNC machining job.

**Why this project?** I used to teach STEM classes in high schools, and engineer projects with kids and teenagers. Every tool is used in the workshop, screwdrivers, arduinos, computers, tablesaws, 3d printers… except for one: the dreaded CNC machine. Which is a shame, as it is the most expensive awesome piece of kit the students have access to. The reason: it’s scary and way less approachable than the afromentioned.

Its fitting, tries to solve a problem in the engineering learning community whilst being a learning project in itself. I can build this project until I’m satisfied for the course, continue building it afterwards to learn new things, and adjust the requirements to fit. Why C? I’m transitioning to computer science from mechanical engineering and C/C++ is a language close enough to the machine (and not machine dependent) that makes it great to learn computer science.

**What are feedrate and speed?** The feedrate is how fast the CNC gantry moves in space x, y, z although z is handled differently depending on the operation type but let’s keep it simple, if you are making that distinction odds are you are using adaptive paths in professional or semi professional environments (a bit out of scope). The spindle speed is how fast the tool is rotating. These are the two most important settigns, and are related to each other through this chipload formula: **feedrate = rpm * chipload * number of teeth**

![Representation of feed rate and spindle speed.](https://www.custompartnet.com/images/calculator/speed-feed-milling-small.png)

In order to eat away material faster by increasing the chipload we also need to spin the tool faster. This is because, each rotation of the tool is capable of cutting a maximum size of material per each tooth, so, if we want to go speed units fast, the tool must rotate at least a certain amount of times per unit of time. If it rotates to slow the tool will crash into the material, creating chatter or even breaking. If it rotates to fast it will rub against the material, generating heat, which blunts the tool and can scorch or gum up the material. Ideally we want to cut nice material sized chips away (dissipating the eat in the chips). We don’t want to crash against the material (producing chuncks, to big of a chip) or rub against it (producing dust, to small of a chip). The **chipload** or **feed per tooth** determines the chip size.

More feedrate equals faster machine times and faster tool rotation yields better surface finish. There are many nuances to this, but it’s the general gist of it. A big factor of CNC machining is thus getting the balance between feed rate and speed right. The calculators and machining programs available use the chipload formula, the problem is one of the inputs must be either the feedrate or spindle speed, which are already parameters most people aren’t comfortable with.

The first version of this program lives in the desktop environment and is interactable through .txt and .csv files, making it infinitely more user friendly than stdin, stdout and stderr. The next step is having a user interface. For the time being the program:

1. **Loads** material information from a .csv file containing: material, tool diameter, chipload, corrective factor; to an hash table in the computers memory;
2. **Reads** an answers.txt file and parses user input to the program, like material, tool diameter, tool number of cutting edges, jog speed, amongst others;
3. **Treats** the users input by removing blank space characters, new line characters, converting strings to numbers where needed, converts numbers to the correct unit for calculations and performs aproximate string matching using the Levenshtein distance;
4. **Calculates** the feed rate and spinde speed via the simplex method, by optimizing for a given set of constraints, a feasible solution region; and a function to maximize, depending on user needs (ie.: optimizing for job speed is different than for surface finish);
5. **Writes** a suggested starting point (feed rate and spindle speed) for the desired job. It can also print other useful information depending on user input, like suggested cutting depth, a checklist for the job, see what materials are currently supported by the program. It also writes error messages for user troubleshooting, error codes for debgging and warning messages for the user.

### Loading Materials into Memory

1. The program opens a .csv file in read mode. It reads the .csv line by line storing each line in a buffer (skiping the header line), then parses each value it finds in that line. If it encounters less values than it expects returns false to main or if it's a non crucial value like a correction factor it automatically resumes with the default of 1.0 (warning the user).

2. Then it builds a data structure, since this program doesn't need to sort or order data, a hash table is the best choice, combining the benefits of both arrays and linked lists. Searching is fast, constant time O(1), or near it if collisions happen (having collisions is not a big deal, since at max one expects materials in the hundreds for a given CNC, collisions will be rare and only affect search time by an indistinguishable amount).

3. We don’t know how many materials, are in the .csv at runtime, so I choose a hash function that hashes evenly throught the available buckets, taking the whole string into account (dbj2). For a fixed number of buckets dbj2 hashes evenly. This is better than alphabetical order because materials don’t follow a even distribution. This implementation still allows for collisions but that’s ok. The fact is there is very little probability of someone with a CNC in need of a material list in the hundreds, let alone in the thousands or millions. Hash function:

```
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
```
### Parsing User Input

This problem was deceivingly more complicated than what appeared at first. In an app with user interface input is usually handled with toggle buttons, select and click, drop menus, etc. in a .txt file however it’s an open minefield of infinite input possibilities; they are just text prompts.

If the user writes **wood** but the material in our hash table is called **Wood** the search will return nothing of use. This is easy, just uppercase, or lowercase everything and then compare. But what if the user writes: **Wo0d**? the *0* could be deleted from the string, do the same deletion in the hash table search, uppercase everything, compare and voila. We could also get rid of new line characters, blank space characters. And if it’s a number? For example, the user enters tool number of teeth: 3, we could do the same and instead of uppercasing use atof or atoi and transform the string into an integer or float. With two helpful functions we could even handle cases like: Enter tool diameter: 3mm, one function gets me the integer 3 (or float) and the other gets me “mm” rid of spaces and non-alphabetical characters.

A context specific problem would be how to handle fractions and different units. CNC gear comes in many different flavors: *metric* and *imperial*. Imperial values are usually represented as fractions. The function that gets me the number can atoi all the numbers until encounters NULL or the “/” character and atoi the rest as a denominator, do some clever math and that will do the trick.

But now the most difficult case, that’s also the most serious and common of all. What if the input the user writes doesn’t match anything in our hash table? Starting with a soft example: user wants to cut **aluminum**, but the table has **aluminium**. We must take care of this… printing a table of supported materials could help, but the user obviously wants **aluminium**, printing a table could is good if **aluminium** is not at all in the table and he should add that or machine a different material. This calls for approximate string-matching, for this problem the Levenshtein distance was the approach. Basically, the computer takes two strings and compares them, each time it needs to delete, insert or substitute it increments the distance by +1. The word that’s closer in distance to the user input is the match. A max distance can be specified so that the matches aren’t ridiculous and even if the distance is acceptable the match can be sketchy, so it must be printed in the results what the computer thinks the user wants for confirmation. Additionally, although not implemented, two searches could be made, one with the exact string the user prompted and another with approximate string matching, allowing for better accuracy, and potencial faster searches but not strictly needed.

1. Open the input file (just like in load materials). Tell the program where the user input is located and save it in variables. But now the variables could be and most likely will be full of garbage.

2. Remove new line characters and whitespace characters;

3. If it's a string keep only alfabetical characters, if it's a number convert it to a number;

4. If any non critical input is left empty or is garbage the calculation will resume with a default, warning the user; else returns with error.

5. Find the best match for material and units the user specified.

code snippets:

```
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

    // Rest of the code...
```

```
/**
     * Clean and extract numerical values from strings representing material, tool unit, tool diameter, tool teeth, job quality, and output units.
     */
    char tool_unit[strlen(tool) + 1];
    strcpy(tool_unit, tool);
    CleanString(material);
    CleanString(tool_unit);
    float tool_diameter = CleanNumber(tool);
    float tool_z = CleanNumber(tool_teeth);
    float speed = CleanNumber(job_quality);
    CleanString(out_unit);
```

```
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
```

```
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
```

### Calculating Chipload

This is the main algorithm. If the user left something blank or with a strange value, but is something that could be guessed the program can run in a default state and write a warning message to the user like: The tool number of teeth “10” doesn’t seem right, are you sure? We calculated using “2” teeth, the most common type of tool. Be sure of the number of teeth before starting your CNC job.

Also we need to convert units, this is a relatively easy problem, but that are some important considerations. For the algorithm what units should be chosen? If it needs to be easily readable for engineers using formulas and units in SI or the countries system could be the safe bet, making the code accessible and easy to spot mistakes where constants live.
Here I went with **rpm** and **mm/m** not because they are the most common or even the most readable units, but because they are both in *minutes* meaning I don’t need to convert anything inside the algorithm and also, they produce the biggest values in modulus, facilitating converting back to the units the user wants displayed, and the math can be interily done in integers (and casted to float if later needed) aligning better with the nature of the problem, since the difference between a feed rate of 500mm/m and 500.1mm/m is irrelevant but from 1.64 feet/m to 1.71 feet/m there is a very substantial difference.

1. **writting the problem**: If the problem at hand needs to be written down genericly it would be something like this: *for the constraints x between two values, and y between two values, where the solution is somewhere between y = constant a * x and y = constant b * x. What is the ideal point x, y that satisfies this optimization equation?* This is called linear programming, along with solving linear equations it’s one of the most common problems in computer science. The algorithm used is the **Simplex Method**.

2. **What optimization equation are we trying to satisfy**? Well it depends from case to case (foreshadowing a switch). Are we striving for surface finish on the machined part? Minimize machining time? Going for tool life optimization? Be as safe as possible on the machine because I’m still very uncomfortable with a CNC? So making adjustment to the conditions that define a feasible solution region and changing the function to optimize will satisfy each case. After that the results are converted to the units the user specified and are printed to a .txt file alongside with warnings and other useful information.


```
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

   // rest of the code...
```
### Writting Results

Writes the results to a .txt file, it also writes warnings and errors for user troubleshooting. All the tools created live in the same file, allowing for ease comparising, also it writes in append mode, making it very easy for the user to just delete what’s not needed, if the file in of itself. Here's a code snippet:

```
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
```
### Considerations
Packaging everything together was a challenge, even though the code isn’t huge, or overly complex, this is my first real biggish project. There are a considerable amount of user inputs that are then filtered, compared, manipulated, etc. In order to minimize the amount of variables, where possible (and where I was capable) the arguments passed functions are pointers, allocating and reallocating memory as needed. This makes the code way more readable and uses less memory.

How to organize the .c files? What should live in the header .h file? What should be global variables? How to comment my code. Is this readable enough? Will I need this function in a later project, how universal should I make it now? Maybe I just refactor it later.

But the worst off all:
**Naming things**


#### **!Beware!**
This program is fully working and easy to modify, but the parameters it works with weren't yet tested. You should use your own materials Table with chipload information. Furthermore the several cases for which it calculates (max finish, finish, balanced, speed, max speed, noob mode) need tuning.

### **Credit and Aknowledgements**

**Book 1** -> The C programming language, Brian Kernighan and Dennis Ritchie

**Book 2** -> The Algorithm Design Manual, Steven Skiena

**Aproximate string matching** -> Levenshtein distance, Levenshtein

**Hash function** -> djb2, Dan Bernstein

**Linear programming** -> simplex method, George, B Dantzig

**General questions** -> chatGPT (GPT 3.5 and 4.0)

**General questions and coding setup** -> Inês Almeida (senior C/C++ developer)

**Special Thanks** -> CS50 staff, you are amazing!!!
