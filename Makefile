# Compiler and flags
CC = g++        # Change 'gcc' to your preferred compiler
CFLAGS = -Wall -Ofast -ffast-math -flto # Add your compiler flags here, e.g., -O2 for optimization
LDFLAGS =       # Add any linker flags here

# Source files, object files, and executable name
SRC = main.cc    # List your source files here
OBJ = $(SRC:.cc=.o) # Generate object file names from source file names
EXECUTABLE = main  # Name of the executable

# Header files
HEADERS = camera.hh color.hh hittable_list.hh hittable.hh interval.hh material.hh ray.hh rtweekend.hh sphere.hh vec3.hh quad.hh triangle.hh    # List your header files here

# Targets and rules
all: $(EXECUTABLE)

ppm: $(EXECUTABLE)
	./main

$(EXECUTABLE): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLE)
