#include "timez.h"

int main(int argc, char** argv) {
    handleArguments(argc, argv);

    executeCommand();

    measureResources();

    printResourceUsage();

    if (verbose) {
        printExtraInfo();
    }

    cleanup();
    return 0;
}
