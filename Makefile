# Compiler
CXX = g++
CC = gcc

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
CFLAGS = -Wall -Wextra -pthread

# Include directories
INCLUDES = -Iincludes

# Source directories
SRCDIR = src
INCDIR = includes

# Find all .cpp, .c files in src/ directory
CXXSRCS = $(wildcard $(SRCDIR)/*.cpp)
CSRCS = $(wildcard $(SRCDIR)/*.c)

# Object files
CXXOBJS = $(CXXSRCS:.cpp=.o)
COBJS = $(CSRCS:.c=.o)

# Output binary
TARGET = concurrent_structs

# Default target
all: $(TARGET)

# Link and create the binary
$(TARGET): $(CXXOBJS) $(COBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(CXXOBJS) $(COBJS)

# Compile .cpp files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile .c files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean the build
clean:
	rm -f $(TARGET) $(CXXOBJS) $(COBJS)

# Phony targets
.PHONY: all clean
