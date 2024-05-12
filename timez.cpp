#include <iostream>
#include <cstdlib>
#include <chrono>                              // For high accuracy wall-clock
#include <sys/resource.h>                      // For rusage
#include <sys/time.h>                          // For timeval

using namespace std;

typedef struct rusage rusage;

int executeCommand(const char *);
int measureResource(int, rusage&);
void printUsage(int, rusage&);

int main(int argc, char *argv[]) {
    // Make sure the argument count is correct
    if (argc != 2) {
        cerr << "Usage: timez  <command>" << endl;
        cerr << "Make sure that there is just 1 argument." << endl;
        return 1;
    }

    // Variable to measure resource usage
    rusage usage;

    // Execute the commands and store the duration
    int duration = executeCommand(argv[1]);
    if (duration == -1) {
        return -1;
    }

    // Measure the resource and check if they are measured correctly
    if (!measureResource(RUSAGE_CHILDREN, usage)) {
        cerr << "Unable to measure resources.\nSomething terribly wrong has happened." << endl;
    }

    // Print the measured usage
    printUsage(duration, usage);

    return 0;
}

// Execute the commands
int executeCommand(const char* command) {
    auto start = chrono::high_resolution_clock::now();

    // Execute the command and store return code
    int ret = system(command);

    auto end = chrono::high_resolution_clock::now();

    // Check the return code
    if (ret != 0) {
        cerr << "Failed to execute command." << endl;
        return -1;
    }

    // Measure the time duration
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    // Returns duration in microsecond
    return duration.count();
}

// Measure the used resources
int measureResource(int who, rusage& usage) {
    // Get the usage and store the return code
    int ret = getrusage(who, &usage);

    // Check if getrusage() is succesful
    return ret == 0;
}

// Print the measured resources
void printUsage(int duration, rusage& usage) {
    if (usage.ru_maxrss < 10000) 
        cout << "\nMemory used                                -> " << usage.ru_maxrss << " KB" << endl;
    else
        cout << "\nMemory used                                -> " << static_cast<float>(usage.ru_maxrss) / 1000 << " MB" << endl;

    if (duration < 1000000)
        cout << "Runtime                                    -> " << static_cast<float>(duration) / 1000 << " ms" << endl;
    else
        cout << "Runtime                                    -> " << static_cast<float>(duration) / 1000000 << " s" << endl;

    cout << "CPU time used in user mode                 -> " << static_cast<float>(usage.ru_utime.tv_usec) / 1000 << " ms" << endl;
    cout << "CPU time used in kernel mode               -> " << static_cast<float>(usage.ru_stime.tv_usec) / 1000 << " ms" << endl;
    cout << "Number of input block(s)                   -> " << usage.ru_inblock << endl;
    cout << "Number of output block(s)                  -> " << usage.ru_oublock << endl;
}
