#ifndef TIMEZ_H
#define TIMEZ_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "args.h"
#include "utils.h"

extern std::ostream* outputStream;
extern std::chrono::steady_clock::time_point start_time;
extern std::chrono::steady_clock::time_point end_time;
extern std::chrono::microseconds runtime;
extern struct rusage usage; 

// Function to execute given command
void executeCommand();

// Function to measure resources
void measureResources();

// Function to print resource usage
void printResourceUsage();

// Function to print extra information when verbose is enabled
void printExtraInfo();

#endif // TIMEZ_H
