#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <string>

#include "crash_detector.hpp"
#include "evidence_collector.hpp"
#include "crash_report.hpp"
#include "gdb_analyzer.hpp"
#include "recovery_manager.hpp"
#include "crashmon_interface.hpp"

std::string findCoreFile(pid_t pid) {

    std::string filename =
        "core." + std::to_string(pid);

    std::ifstream file(filename);

    if (file.good()) {
        return filename;
    }

    return "";
}


int main(int argc, char* argv[]) {

    CrashMonInterface::initialize();

    std::string executable = "./crash_test";

    if (argc > 1) {
        executable = argv[1];
    }

    pid_t pid = fork();

    if (pid < 0) {

        std::cerr
            << "Failed to create process\n";

        return 1;
    }


    if (pid == 0) {

        std::cout
            << "Starting crash test application...\n";

        execl(
            executable.c_str(),
	    executable.c_str(),
            nullptr
        );

        std::cerr
            << "Failed to execute crash_test\n";

        return 1;
    }


    std::cout
        << "Monitoring process PID: "
        << pid
        << "\n";


    /*
     * Give the child process time to
     * execute crash_test.
     */

    sleep(1);


    /*
     * Collect evidence while the process
     * is still alive.
     */

    std::string processName =
        EvidenceCollector::getProcessName(pid);

    std::string parentPid =
        EvidenceCollector::getParentPid(pid);

    std::string processState =
        EvidenceCollector::getProcessState(pid);

    std::string memoryUsage =
        EvidenceCollector::getMemoryUsage(pid);

    std::string virtualMemory =
        EvidenceCollector::getVirtualMemory(pid);

    std::string threadCount =
        EvidenceCollector::getThreadCount(pid);

    std::string commandLine =
        EvidenceCollector::getCommandLine(pid);


    std::cout << "\n";

    std::cout
        << "========== PRE-CRASH EVIDENCE ==========\n";

    std::cout
        << "Process Name    : "
        << processName
        << "\n";

    std::cout
        << "Parent PID      : "
        << parentPid
        << "\n";

    std::cout
        << "Process State   : "
        << processState
        << "\n";

    std::cout
        << "Memory Usage    : "
        << memoryUsage
        << "\n";

    std::cout
        << "Virtual Memory  : "
        << virtualMemory
        << "\n";

    std::cout
        << "Threads         : "
        << threadCount
        << "\n";

    std::cout
        << "Command Line    : "
        << commandLine
        << "\n";


    /*
     * Wait for the monitored process
     * to terminate.
     */

    int status;

    waitpid(pid, &status, 0);


    /*
     * Check whether the process crashed.
     */

    if (CrashDetector::crashed(status)) {

        int signal =
            WTERMSIG(status);

	CrashMonInterface::recordCrash(
    	    pid,
            signal,
    	    CrashDetector::getSignalName(signal)
	);

        std::string signalName =
            CrashDetector::getSignalName(signal);


        /*
         * Find the core dump generated
         * for this process.
         */

        std::string coreFile =
            findCoreFile(pid);


        std::string gdbAnalysis;


        /*
         * Run automatic GDB analysis
         * if the core dump exists.
         */

        if (!coreFile.empty()) {

            std::cout
                << "\nRunning GDB crash analysis...\n";

            gdbAnalysis =
                GDBAnalyzer::analyze(
                    "./crash_test",
                    coreFile
                );
        }
        else {

            gdbAnalysis =
                "Core dump not found";
        }


        /*
         * Generate the final forensic report.
         */

        bool reportCreated =
            CrashReport::generate(
                pid,
                signal,
                signalName,
                processName,
                parentPid,
                processState,
                memoryUsage,
                virtualMemory,
                threadCount,
                commandLine,
                gdbAnalysis
            );


        if (reportCreated) {

            std::cout
                << "\nCrash report generated successfully.\n";
        }

	RecoveryManager::restart(executable, 3);


        std::cout << "\n";

        std::cout
            << "========================================\n";

        std::cout
            << "           CRASH DETECTED\n";

        std::cout
            << "========================================\n";

        std::cout
            << "PID    : "
            << pid
            << "\n";

        std::cout
            << "Signal : "
            << signalName
            << "\n";

        std::cout
            << "Core   : "
            << (coreFile.empty()
                    ? "Not Found"
                    : coreFile)
            << "\n";

        std::cout
            << "========================================\n";
    }


    else if (WIFEXITED(status)) {

        std::cout
            << "\nProcess exited normally\n";

        std::cout
            << "Exit code: "
            << WEXITSTATUS(status)
            << "\n";
    }


    return 0;
}
