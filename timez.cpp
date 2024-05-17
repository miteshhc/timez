// TODO: Add IO read/write speed

#include <cstddef>
#include <iostream>
#include <cerrno>                                   // For EFAULT, EINVAL
#include <cstring>                                  // For strcmp
#include <cstdlib>
#include <chrono>                                   // For high-precision wall-clock
#include <fstream>
#include <string>
#include <unistd.h>                                 // For execl, fork, usleep
#include <sys/resource.h>                           // For getrusage
#include <sys/wait.h>                               // For waitpid
#include <signal.h>                                 // For SIGKILL, kill
#include <vector>

using namespace std;

typedef struct rusage rusage;


// Function prototypes
void help();
int executeCommand(const char *, int);
int measureResource(int);
void printUsage(long long int, string);


// Global Variables
bool verbose = false;
bool outputFile = false;
rusage usage;

int main(int argc, char *argv[]) {
    // Check if arguments are provided correctly
    if (argc < 2) {
        help();
    }

    // Variable to store command
    string command;

    // Varible to store output file name
    string outputFileName;

    // executeFor flag
    int executeFor = -1;

    // Vector to store the command and its arguments
    vector<string> commandArgs;

    // Loop to check flags
    for (int i = 1; i < argc; i++) {
        // Check if verbose flag exists
        if (strcmp("-v", argv[i]) == 0 || strcmp("--verbose", argv[i]) == 0) {
            verbose = true;
        }

        // Check if fixed-interval flag exists
        else if (strcmp("-f", argv[i]) == 0 || strcmp("--fixed-interval", argv[i]) == 0) {
            if (i + 1 < argc) {
                executeFor = atoi(argv[++i]);
            } else {
                help();
            }
        }

        // Check if output-file flag exists
        else if (strcmp("-o", argv[i]) == 0 || strcmp("--output-file", argv[i]) == 0) {
            if (i + 1 < argc) {
                outputFile = true;
                outputFileName = argv[++i];
            } else {
                help();
            }
        }

        // Check if help flag exists
        else if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0) {
            help();
        }

        // Otherwise, it is part of the command to execute
        else {
            commandArgs.push_back(argv[i]);
        }
    }

    // Check if a command was provided
    if (commandArgs.empty()) {
        cerr << "No command provided to execute." << endl;
        help();
    }


    // Construct the command string
    for (const auto& arg : commandArgs) {
        command += arg + " ";
    }

    // Execute the command and store the duration
    long long int duration = executeCommand(command.c_str(), executeFor);
    if (duration == -1) {
        return -1;
    }

    // Measure the resource and check if they are measured correctly
    if (!measureResource(RUSAGE_CHILDREN)) {
        cerr << "Unable to measure resources.\nSomething terribly wrong has happened." << endl;
    }

    // Print the measured usage
    printUsage(duration, outputFileName);

    return EXIT_SUCCESS;
}

void help()
{
    cout << "Usage: timez <command> (flag) " << endl;
    cout << "\nFlags:" << endl;
    cout << "\t-f <execute for>                   --fixed-interval <execute for>" << endl;
    cout << "\t-h                                 --help" << endl;
    cout << "\t-o <filename>                      --output-file <filename>" << endl;
    cout << "\t-v                                 --verbose" << endl;
    cout << "\nNOTE: The run-time is almost always +1ms of original time while using fixed-interval flag." << endl;
    exit(EXIT_FAILURE);
}

// Execute the commands
int executeCommand(const char* command, int executeFor) {
    auto start = chrono::high_resolution_clock::now();

    // Fork a new process
    pid_t pid = fork();


    // Check if fork is successful
    if (pid < 0) {
        cerr << "Failed to fork a new process." << endl;
        return -1;
    }

    if (pid == 0) {
        // Execute the command using exec
        execl("/bin/sh", "sh", "-c", command, NULL);            // `/bin/sh` - according to POSIX standards

        // If execl returns, there was an error
        cerr << "Failed to execute command." << endl;
        exit(EXIT_FAILURE);
    }

    // Sleep for the specified duration before killing the child process
    if (executeFor != -1) {

        // milliseconds to microseconds
        usleep(executeFor * 1000);

        // Kill the child process
        kill(pid, SIGKILL);

        // Wait for a short time to ensure the process has terminated
        // usleep(1000); // 1 milliseconds
    }

    int status;

    // Make parent process wait till the execution of child process
    waitpid(pid, &status, 0);

    auto end = chrono::high_resolution_clock::now();

    // Measure the time duration
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    // Returns duration in microseconds
    return duration.count();
}

// Measure the used resources
int measureResource(int who) {
    // Get the usage and store the return code
    int ret = getrusage(who, &usage);

    if (ret == EFAULT) {
        cerr << "Cannot measure resources due to inaccessible address space." << endl;
        exit(EXIT_FAILURE);
    } else if (ret == EINVAL) {
        cerr << "Invalid CHILD process" << endl;
        exit(EXIT_FAILURE);
    }

    // Check if getrusage() was successful
    return ret == 0;
}

// Print the measured resources
void printUsage(long long int duration, string outputFileName)
{
    // Print to file
    if (outputFile) {
        ofstream outFile(outputFileName);

        if (!outFile) {
            cerr << "Unable to open file " << outputFileName << endl;
            exit(EXIT_FAILURE);
        }

        if (usage.ru_maxrss < 10000) 
            outFile << "\nMemory used                                -> " << usage.ru_maxrss << " KB" << endl;
        else
            outFile << "\nMemory used                                -> " << static_cast<float>(usage.ru_maxrss) / 1000 << " MB" << endl;

        if (duration < 1000000)
            outFile << "Runtime                                    -> " << static_cast<float>(duration) / 1000 << " ms" << endl;
        else
            outFile << "Runtime                                    -> " << static_cast<float>(duration) / 1000000 << " s" << endl;

        if (verbose) {
            outFile << "CPU time used in user mode                 -> " << static_cast<float>(usage.ru_utime.tv_usec) / 1000 << " ms" << endl;
            outFile << "CPU time used in kernel mode               -> " << static_cast<float>(usage.ru_stime.tv_usec) / 1000 << " ms" << endl;
            outFile << "Page reclaims (Soft-Page Fault)            -> " << usage.ru_minflt << endl;
            outFile << "Page faults (Hard-Page Fault)              -> " << usage.ru_majflt << endl;
            outFile << "Number of input block(s)                   -> " << usage.ru_inblock << endl;
            outFile << "Number of output block(s)                  -> " << usage.ru_oublock << endl;
            outFile << "Voluntary context switches                 -> " << usage.ru_nvcsw << endl;
            outFile << "Involuntary context switches               -> " << usage.ru_nivcsw << endl;
        }

        outFile.close();
    } 

    // Print to stdout
    else {
        if (usage.ru_maxrss < 10000) 
            cout << "\nMemory used                                -> " << usage.ru_maxrss << " KB" << endl;
        else
            cout << "\nMemory used                                -> " << static_cast<float>(usage.ru_maxrss) / 1000 << " MB" << endl;

        if (duration < 1000000)
            cout << "Runtime                                    -> " << static_cast<float>(duration) / 1000 << " ms" << endl;
        else
            cout << "Runtime                                    -> " << static_cast<float>(duration) / 1000000 << " s" << endl;

        if (verbose) {
            cout << "CPU time used in user mode                 -> " << static_cast<float>(usage.ru_utime.tv_usec) / 1000 << " ms" << endl;
            cout << "CPU time used in kernel mode               -> " << static_cast<float>(usage.ru_stime.tv_usec) / 1000 << " ms" << endl;
            cout << "Page reclaims (Soft-Page Fault)            -> " << usage.ru_minflt << endl;
            cout << "Page faults (Hard-Page Fault)              -> " << usage.ru_majflt << endl;
            cout << "Number of input block(s)                   -> " << usage.ru_inblock << endl;
            cout << "Number of output block(s)                  -> " << usage.ru_oublock << endl;
            cout << "Voluntary context switches                 -> " << usage.ru_nvcsw << endl;
            cout << "Involuntary context switches               -> " << usage.ru_nivcsw << endl;
        }
    }
}
