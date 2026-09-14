# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Includes and libraries — differ by platform, so branch on uname.
# Linux and WSL (WSL reports "Linux" here too) link against system OpenGL + dl.
# macOS has no libGL/libdl to link against; it uses Apple's frameworks instead,
# and needs Homebrew's include/lib paths for GLFW.
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    # Homebrew's install prefix differs by Mac architecture:
    #   Apple Silicon (M1/M2/M3/M4, arm64) -> /opt/homebrew
    #   Intel (x86_64)                     -> /usr/local
    # Picking the wrong one means GLFW's headers/lib silently aren't found.
    UNAME_M := $(shell uname -m)
    ifeq ($(UNAME_M),arm64)
        HOMEBREW_PREFIX = /opt/homebrew
    else
        HOMEBREW_PREFIX = /usr/local
    endif

    INCLUDES = -I. -I$(HOMEBREW_PREFIX)/include
    LDFLAGS  = -L$(HOMEBREW_PREFIX)/lib
    LIBS     = -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    CXXFLAGS += -DGL_SILENCE_DEPRECATION
else
    INCLUDES = -I.
    LDFLAGS  =
    LIBS     = -lglfw -ldl -lGL
endif

# Source and object files
SRC = main.cpp glad.c
OBJ = main.o glad.o

# Output executable name — must match what the assignment instructions
# and submission checklist require.
TARGET = assignment_1

# Default make target
all: $(TARGET)

# Compile the program
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS) $(LIBS)

# Compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source files (for glad.c)
%.o: %.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build and immediately run the executable
run: $(TARGET)
	./$(TARGET)

# Clean compiled files, but DO NOT delete glad.c!
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean
