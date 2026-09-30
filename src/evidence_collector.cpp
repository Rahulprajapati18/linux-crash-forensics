#include "evidence_collector.hpp"

#include <fstream>
#include <sstream>

static std::string getValue(int pid, const std::string& key) {

    std::string path =
        "/proc/" + std::to_string(pid) + "/status";

    std::ifstream file(path);

    if (!file.is_open()) {
        return "Unavailable";
    }

    std::string line;

    while (std::getline(file, line)) {

        if (line.rfind(key, 0) == 0) {

            size_t colon = line.find(':');

            if (colon != std::string::npos) {
                return line.substr(colon + 1);
            }
        }
    }

    return "Unavailable";
}

std::string EvidenceCollector::getProcessName(int pid) {
    return getValue(pid, "Name");
}

std::string EvidenceCollector::getParentPid(int pid) {
    return getValue(pid, "PPid");
}

std::string EvidenceCollector::getProcessState(int pid) {
    return getValue(pid, "State");
}

std::string EvidenceCollector::getMemoryUsage(int pid) {
    return getValue(pid, "VmRSS");
}

std::string EvidenceCollector::getVirtualMemory(int pid) {
    return getValue(pid, "VmSize");
}

std::string EvidenceCollector::getThreadCount(int pid) {
    return getValue(pid, "Threads");
}

std::string EvidenceCollector::getCommandLine(int pid) {

    std::string path =
        "/proc/" + std::to_string(pid) + "/cmdline";

    std::ifstream file(path);

    if (!file.is_open()) {
        return "Unavailable";
    }

    std::string command;

    std::getline(file, command, '\0');

    return command;
}
