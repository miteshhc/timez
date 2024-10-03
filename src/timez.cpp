#include <iostream>
#include <thread>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/wait.h>

#include "timez.h"

std::chrono::steady_clock::time_point start_time;
std::chrono::steady_clock::time_point end_time;
std::chrono::microseconds runtime;
std::ostream* outputStream = &std::cout;
struct rusage usage;

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

        std::thread durationThread;

        // Check if duration is greater than 0.0
        if (duration > 0.0) {
            durationThread = std::thread([pid]() {
                std::this_thread::sleep_for(
                    std::chrono::duration<double>(duration));
                    if (kill(pid, 0) == 0) {
                        std::cerr << "Duration exceeded. Killing process " << pid
                                << std::endl;
                        kill(pid, SIGKILL);
                    }
                });
        }

        start_time = std::chrono::steady_clock::now();
        waitpid(pid, &status, 0);
        end_time = std::chrono::steady_clock::now();

        // If the duration thread was created, join it
        if (durationThread.joinable()) {
            durationThread.join();
        }

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
        *outputStream << "Memory used                     \t\t\t\t-->\t\t" << usage.ru_maxrss / 1024 << " MB" << std::endl;
    } else {
        *outputStream << "Memory used                     \t\t\t\t-->\t\t" << usage.ru_maxrss << " KB" << std::endl;
    }

    // Convert microseconds to seconds
    double seconds = runtime.count() / 1e6;
    
    if (seconds > 1.0) {
        *outputStream << "Runtime                         \t\t\t\t-->\t\t" << seconds << " s" << std::endl;
    } else if (runtime.count() > 1000) {
        *outputStream << "Runtime                         \t\t\t\t-->\t\t" << runtime.count() / 1000 << " ms" << std::endl;
    } else {
        *outputStream << "Runtime                         \t\t\t\t-->\t\t" << runtime.count() << " us" << std::endl;
    }

    *outputStream << std::endl;
}

void printExtraInfo() {
    // User CPU time in microseconds
    double userSeconds = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
    if (userSeconds > 1.0) {
        *outputStream << "CPU time used in user mode      \t\t\t\t-->\t\t" << userSeconds << " s" << std::endl;
    } else if (usage.ru_utime.tv_usec > 1000) {
        *outputStream << "CPU time used in user mode      \t\t\t\t-->\t\t" << usage.ru_utime.tv_usec / 1000 << " ms" << std::endl;
    } else {
        *outputStream << "CPU time used in user mode      \t\t\t\t-->\t\t" << usage.ru_utime.tv_usec << " us" << std::endl;
    }

    // System CPU time in microseconds
    double systemSeconds = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
    if (systemSeconds > 1.0) {
        *outputStream << "CPU time used in system mode    \t\t\t\t-->\t\t" << systemSeconds << " s" << std::endl;
    } else if (usage.ru_stime.tv_usec > 1000) {
        *outputStream << "CPU time used in system mode    \t\t\t\t-->\t\t" << usage.ru_stime.tv_usec / 1000 << " ms" << std::endl;
    } else {
        *outputStream << "CPU time used in system mode    \t\t\t\t-->\t\t" << usage.ru_stime.tv_usec << " us" << std::endl;
    }

    *outputStream << "Page reclaims (Soft-Page Fault) \t\t\t\t-->\t\t" << usage.ru_minflt << std::endl;
    *outputStream << "Page faults (Hard-Page Fault)   \t\t\t\t-->\t\t" << usage.ru_majflt << std::endl;
    *outputStream << "Number of input block(s)        \t\t\t\t-->\t\t" << usage.ru_inblock << std::endl;
    *outputStream << "Number of output block(s)       \t\t\t\t-->\t\t" << usage.ru_oublock << std::endl;
    *outputStream << "Voluntary context switches      \t\t\t\t-->\t\t" << usage.ru_nvcsw << std::endl;
    *outputStream << "Involuntary context switches    \t\t\t\t-->\t\t" << usage.ru_nivcsw << std::endl;

    *outputStream << std::endl;
}
