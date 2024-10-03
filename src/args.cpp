#include <iostream>
#include "utils.h"
#include "args.h"
#include "cxxopts.hpp"

double duration = 0.0;
std::string outStream;
bool verbose = false;
std::vector<std::string> command;

void handleArguments(int argc, char** argv) {
    cxxopts::Options options("timez", "A simple utility for measuring the execution time and resource usage of commands.");

    options.add_options()
        ("d,duration", "Execute command for specific duration", cxxopts::value<double>()) // Changed to double
        ("h,help", "Print help message")
        ("o,out", "Output stream", cxxopts::value<std::string>())
        ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"));

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
