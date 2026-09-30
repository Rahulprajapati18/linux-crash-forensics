#ifndef CRASH_DETECTOR_HPP
#define CRASH_DETECTOR_HPP

#include <string>

class CrashDetector {
public:
    static bool crashed(int status);
    static std::string getSignalName(int signal);
};

#endif
