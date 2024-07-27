# Compiler and compiler flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++20 -g

# Linker flags, some common ones
LDFLAGS = -lcs50 -lm

# Output executable name
TARGET = chipload

# Source files
SOURCES = main.cpp read.cpp helpers.cpp load.cpp write.cpp simplex.cpp

# Object files (by replacing .cpp with .o in the source files)
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to link the object files to create the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Rule to compile source files into object files
%.o: %.cpp chipload.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove compiled files
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: all clean
