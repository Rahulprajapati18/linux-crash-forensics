#include "crash_report.hpp"

#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

bool CrashReport::generate(
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
) {

    std::time_t currentTime =
        std::time(nullptr);

    std::tm* localTime =
        std::localtime(&currentTime);

    std::ostringstream filename;

    filename
        << "reports/crash_"
        << std::put_time(
               localTime,
               "%Y-%m-%d_%H-%M-%S"
           )
        << ".txt";

    std::ofstream report(
        filename.str()
    );

    if (!report.is_open()) {

        std::cerr
            << "Failed to create crash report\n";

        return false;
    }


    report
        << "========================================\n";

    report
        << "       LINUX CRASH FORENSICS REPORT\n";

    report
        << "========================================\n\n";


    report
        << "Crash Time       : "
        << std::put_time(
               localTime,
               "%Y-%m-%d %H:%M:%S"
           )
        << "\n\n";


    report
        << "Application      : "
        << processName
        << "\n";

    report
        << "PID              : "
        << pid
        << "\n";

    report
        << "Parent PID       : "
        << parentPid
        << "\n\n";


    report
        << "Crash Signal     : "
        << signalName
        << "\n";

    report
        << "Signal Number    : "
        << signal
        << "\n\n";


    report
        << "Process State    : "
        << processState
        << "\n";

    report
        << "Memory Usage     : "
        << memoryUsage
        << "\n";

    report
        << "Virtual Memory   : "
        << virtualMemory
        << "\n";

    report
        << "Threads          : "
        << threadCount
        << "\n";

    report
        << "Command Line     : "
        << commandLine
        << "\n\n";


    report
        << "========== GDB ANALYSIS ==========\n\n";

    report
        << gdbAnalysis
        << "\n";


    report
        << "========================================\n";


    report.close();


    std::cout
        << "Report saved to: "
        << filename.str()
        << "\n";


    return true;
}
