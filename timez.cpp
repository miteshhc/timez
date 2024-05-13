#include <cerrno>
#include <iostream>                                 // For cerr, cout, endl
#include <cstring>                                  // For strcmp
#include <cstdlib>                                  // For EXIT_FAILURE, EXIT_SUCCESS, NULL, atoi, exit
#include <chrono>                                   // For high-precision wall-clock
#include <fstream>                                  // For writing to file
#include <unistd.h>                                 // For execl, fork, usleep
#include <sys/resource.h>                           // For getrusage
#include <sys/wait.h>                               // For waitpid
#include <signal.h>                                 // For SIGKILL, kill

using namespace std;

typedef struct rusage rusage;

void help();
int executeCommand(const char *, int);
int measureResource(int, rusage&);
void printUsage(int, rusage&, string, bool, bool);

int main(int argc, char *argv[]) {

    // Check if arguments are provided correctly
    if (argc < 2) {
        help();
    }

    bool verbose = false;
    bool outputFile = false;
    string outputFileName;
    int executeFor = -1;

    // Loop the check flags
    for (int i = 2; i < argc; i++) {
        // Check if verbose flag exists
        if (strcmp("-v", argv[i]) == 0 || strcmp("--verbose", argv[i]) == 0) {
            verbose = true;
        }

        // Check if fixed-interval flag exists
        if (strcmp("-f", argv[i]) == 0 || strcmp("--fixed-interval", argv[i]) == 0) {
            if (i + 1 < argc) {
                executeFor = atoi(argv[i+1]);
            }
            else 
                help();
        }

        // Check if output-file flag exists
        if (strcmp("-o", argv[i]) == 0 || strcmp("--output-file", argv[i]) == 0) {
            if (i + 1 < argc) {
                outputFile = true;
                outputFileName = argv[i+1];
            }
            else
                help();
        }

        // Check if help flag exists
        if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0) {
            help();
        }
    }

    // Variable to measure resource usage
    rusage usage;

    // Execute the commands and store the duration
    int duration = executeCommand(argv[1], executeFor);
    if (duration == -1) {
        return -1;
    }

    // Measure the resource and check if they are measured correctly
    if (!measureResource(RUSAGE_CHILDREN, usage)) {
        cerr << "Unable to measure resources.\nSomething terribly wrong has happened." << endl;
    }

    // Print the measured usage
    printUsage(duration, usage, outputFileName, verbose, outputFile);

    return EXIT_SUCCESS;
}

// If arguments are not provided correctly
void help()
{
        cout << "Usage: timez <command> (flag) " << endl;
        cout << "\nFlags:" << endl;
        cout << "\t-f <execute for>                   --fixed-interval <execute for>" << endl;
        cout << "\t-h                                 --help" << endl;
        cout << "\t-o <filename>                      --output-file <filename>" << endl;
        cout << "\t-v                                 --verbose" << endl;
        cout << "\nNOTE: The run-time is almost always +10ms of original time and may vary on machines." << endl;
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
        execl("/bin/sh", "sh", "-c", command, NULL);

        // If execl returns, there was an error
        cerr << "Failed to execute command." << endl;
        exit(EXIT_FAILURE);
    }

    // Sleep for the specified duration before killing the child process
    if (executeFor > 0) {
        usleep(executeFor * 1000); // Convert milliseconds to microseconds
        // Kill the child process
        kill(pid, SIGKILL);
        // Wait for a short time to ensure the process has terminated
        usleep(10000); // 10 milliseconds
    }

    int status;

    // Make parent process wait till the execution of child process
    waitpid(pid, &status, 0);

    auto end = chrono::high_resolution_clock::now();

    // Measure the time duration
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    // Returns duration in microsecond
    return duration.count();
}

// Measure the used resources
int measureResource(int who, rusage& usage) {
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
void printUsage(int duration, rusage& usage, string outputFileName, bool verbose, bool saveToFile)
{
    if (saveToFile) {
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
            outFile << "Number of input block(s)                   -> " << usage.ru_inblock << endl;
            outFile << "Number of output block(s)                  -> " << usage.ru_oublock << endl;
        }

        outFile.close();
    } 

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
            cout << "Number of input block(s)                   -> " << usage.ru_inblock << endl;
            cout << "Number of output block(s)                  -> " << usage.ru_oublock << endl;
        }
    }
}
