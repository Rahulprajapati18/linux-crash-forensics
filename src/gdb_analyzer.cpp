#include "gdb_analyzer.hpp"

#include <cstdio>
#include <iostream>

std::string GDBAnalyzer::analyze(
    const std::string& executable,
    const std::string& coreFile
) {
    std::string command =
        "gdb -batch "
        "-ex \"bt\" "
        "-ex \"info registers\" "
        "\"" + executable + "\" "
        "\"" + coreFile + "\" 2>&1";

    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe) {
        return "GDB analysis failed";
    }

    char buffer[256];
    std::string result;

    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }

    pclose(pipe);

    return result;
}
