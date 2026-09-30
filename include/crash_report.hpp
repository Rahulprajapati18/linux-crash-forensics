#ifndef CRASH_REPORT_HPP
#define CRASH_REPORT_HPP

#include <string>

class CrashReport {
public:

    static bool generate(
        int pid,
        int signal,
        const std::string& signalName,
        const std::string& processName,
        const std::string& parentPid,
        const std::string& processState,
        const std::string& memoryUsage,
        const std::string& virtualMemory,
        const std::string& threadCount,
        const std::string& commandLine,
        const std::string& gdbAnalysis
    );
};

#endif
