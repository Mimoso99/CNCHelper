# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Linker flags, some common ones
LDFLAGS = -lcs50 -lm

# Output executable name
TARGET = chipload

# Source files
SOURCES = main.c read.c helpers.c load.c write.c simplex.c

# Object files (by replacing .c with .o in the source files)
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Rule to link the object files to create the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Rule to compile source files into object files
%.o: %.c chipload.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove compiled files
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: all clean
