#ifndef CRASHMON_INTERFACE_HPP
#define CRASHMON_INTERFACE_HPP

#include <string>

class CrashMonInterface {
public:

    static bool initialize();

    static bool recordCrash(
        int pid,
        int signal,
        const std::string& signalName
    );

    static int getCrashCount();

    static std::string getLastCrash();

    static void shutdown();
};

#endif
