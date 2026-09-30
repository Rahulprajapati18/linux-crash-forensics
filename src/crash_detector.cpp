#include "crash_detector.hpp"

#include <sys/wait.h>
#include <csignal>

bool CrashDetector::crashed(int status) {
    return WIFSIGNALED(status);
}

std::string CrashDetector::getSignalName(int signal) {

    switch (signal) {
        case SIGSEGV:
            return "SIGSEGV - Segmentation Fault";

        case SIGABRT:
            return "SIGABRT - Aborted";

        case SIGFPE:
            return "SIGFPE - Arithmetic Exception";

        case SIGILL:
            return "SIGILL - Illegal Instruction";

        case SIGTERM:
            return "SIGTERM - Terminated";

        case SIGKILL:
            return "SIGKILL - Killed";

        default:
            return "Unknown Signal";
    }
}
