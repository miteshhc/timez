#ifndef ARGS_H
#define ARGS_H

#include <string>
#include <vector>

extern double duration;
extern std::string outStream;
extern bool verbose;
extern std::vector<std::string> command;

// Function to handle command line arguments
void handleArguments(int argc, char** argv);

#endif // ARGS_H
