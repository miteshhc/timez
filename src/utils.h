#ifndef UTILS_H
#define UTILS_H

#include <fstream>

extern std::ofstream fileStream;

// Function to exit the program safely after cleanup
void dead(int exitCode);

// Function to clear resources before exiting
void cleanup();

#endif // UTILS_H
