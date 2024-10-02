#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <vector>

// Reference: https://www.man7.org/linux/man-pages/man2/getrusage.2.html
#include <sys/resource.h>
#include <sys/wait.h>

#include "cxxopts.hpp"

void handleArguments(int argc, char** argv);
void executeCommand();
void printResourceUsage();
void printExtraInfo();
void measureResources();
void dead(int exitCode);

std::vector<std::string> command;
double duration;
std::string outStream;
bool verbose;
struct rusage usage;
std::ofstream fileStream;

// Default output stream: std::cout
std::ostream* outputStream = &std::cout;

std::chrono::time_point<std::chrono::steady_clock> start_time;
std::chrono::time_point<std::chrono::steady_clock> end_time;

// Note that microseconds or nanoseconds can be used to for high-precision measurements
std::chrono::microseconds runtime;

int main(int argc, char** argv) {
    handleArguments(argc, argv);
    executeCommand();
    measureResources();

    if (!outStream.empty()) {
        fileStream.open(outStream);
        if (!fileStream) {
            std::cerr << "Error: Unable to open output stream: " << outStream << std::endl;
            dead(1);
        }
        outputStream = &fileStream;
    }

    if (verbose) {
        printResourceUsage();
        printExtraInfo();
    } else {
        printResourceUsage();
    }

    return 0;
}

void dead(int exitCode) {
    if (fileStream.is_open()) {
        fileStream.close();
    }

    std::cerr << "Exiting..." << std::endl;

    exit(exitCode);
}

void handleArguments(int argc, char** argv) {
    cxxopts::Options options("timez", "A simple tool to measure resource usage of command");

    options.add_options()
        ("d,duration", "Execute command for specific duration", cxxopts::value<float>())
        ("h,help", "Print help message")
        ("o,out", "Output stream", cxxopts::value<std::string>())
        ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
        ;

    auto result = options.parse(argc, argv);

    if (result.count("duration")) {
        duration = result["duration"].as<double>();
    }

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        dead(0);
    }

    if (result.count("out")) {
        outStream = result["out"].as<std::string>();
    }

    if (result.count("verbose")) {
        verbose = result["verbose"].as<bool>();
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            break;
        command.push_back(argv[i]);
    }

    if (command.empty()) {
        std::cerr << "Error: No command specified. Use --help for usage." << std::endl;
        dead(1);
    }
}

void executeCommand() {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Failed to fork a new process." << std::endl;
        dead(1);
    }

    if (pid == 0) {
        std::vector<char*> args;
        for (const auto& arg : command) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }

        args.push_back(nullptr);

        if (verbose) {
            std::cout << "Executing command: ";
            for (const auto& arg : command) {
                std::cout << arg << " ";
            }
            std::cout << std::endl;
        }

        execvp(args[0], args.data());

        std::cerr << "Failed to execute command." << std::endl;
        dead(1);
    }
    else {
        int status;

        start_time = std::chrono::steady_clock::now();
        waitpid(pid, &status, 0);
        end_time = std::chrono::steady_clock::now();

        runtime = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        if (verbose) {
            if (WIFEXITED(status)) {
                std::cout << "Command exited with status " << WEXITSTATUS(status) << std::endl;
            }
            else if (WIFSIGNALED(status)) {
                std::cout << "Command terminated by signal " << WTERMSIG(status) << std::endl;
            }
            else if (WIFSTOPPED(status)) {
                std::cout << "Command stopped by signal " << WSTOPSIG(status) << std::endl;
            }
        }
    }
}

void measureResources() {
    int ret = getrusage(RUSAGE_CHILDREN, &usage);

    if (ret == EFAULT) {
        std::cerr << "Cannot measure resources due to inaccessible address space." << std::endl;
        dead(1);
    }
    else if (ret == EINVAL) {
        std::cerr << "Invalid CHILD process" << std::endl;
        dead(1);
    }
    else if (ret != 0) {
        std::cerr << "Unknown error while measuring resources." << std::endl;
        dead(1);
    }
}

void printResourceUsage() {
    *outputStream << std::endl;

    if (usage.ru_maxrss > 1024) {
        *outputStream << "Memory used\t\t\t\t-->\t\t" << usage.ru_maxrss / 1024 << " MB" << std::endl;
    } else {
        *outputStream << "Memory used\t\t\t\t-->\t\t" << usage.ru_maxrss << " KB" << std::endl;
    }

    // Correct conversion for runtime output
    double seconds = runtime.count() / 1e6; // Convert microseconds to seconds
    
    if (seconds > 1.0) {
        *outputStream << "Runtime    \t\t\t\t-->\t\t" << seconds << " s" << std::endl;
    } else if (runtime.count() > 1000) {
        *outputStream << "Runtime    \t\t\t\t-->\t\t" << runtime.count() / 1000 << " ms" << std::endl;
    } else {
        *outputStream << "Runtime    \t\t\t\t-->\t\t" << runtime.count() << " us" << std::endl;
    }

    *outputStream << std::endl;
}

void printExtraInfo() {
    if (usage.ru_utime.tv_usec > 1000) {
        *outputStream << "CPU time used in user mode  \t\t\t\t-->\t\t" << usage.ru_utime.tv_usec / 1000 << " ms" << std::endl;
    } else {
        *outputStream << "CPU time used in user mode  \t\t\t\t-->\t\t" << usage.ru_utime.tv_usec << " us" << std::endl;
    }

    if (usage.ru_stime.tv_usec > 1000) {
        *outputStream << "CPU time used in system mode\t\t\t\t-->\t\t" << usage.ru_stime.tv_usec / 1000 << " ms" << std::endl;
    } else {
        *outputStream << "CPU time used in system mode\t\t\t\t-->\t\t" << usage.ru_stime.tv_usec << " us" << std::endl;
    }

    *outputStream << "Page reclaims (Soft-Page Fault) \t\t\t\t-->\t\t" << usage.ru_minflt << std::endl;
    *outputStream << "Page faults (Hard-Page Fault)   \t\t\t\t-->\t\t" << usage.ru_majflt << std::endl;
    *outputStream << "Number of input block(s)        \t\t\t\t-->\t\t" << usage.ru_inblock << std::endl;
    *outputStream << "Number of output block(s)       \t\t\t\t-->\t\t" << usage.ru_oublock << std::endl;
    *outputStream << "Voluntary context switches      \t\t\t\t-->\t\t" << usage.ru_nvcsw << std::endl;
    *outputStream << "Involuntary context switches    \t\t\t\t-->\t\t" << usage.ru_nivcsw << std::endl;

    *outputStream << std::endl;
}
