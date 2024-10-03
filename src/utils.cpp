#include <fstream>
#include "utils.h"

std::ofstream fileStream;

void dead(int exitCode) {
    cleanup();

    exit(exitCode);
}

void cleanup() {
    if (fileStream.is_open()) {
        fileStream.close();
    }

    // Do more cleanup here
}   
