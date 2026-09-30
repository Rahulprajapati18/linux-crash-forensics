#include "crashmon_interface.hpp"

#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace {

const std::string stateFile =
    "logs/crashmon_state.txt";

int crashCount = 0;

std::string lastCrash = "No crash recorded";

}


bool CrashMonInterface::initialize()
{
    std::ifstream file(stateFile);

    if (file.is_open()) {

        file >> crashCount;

        file.ignore();

        std::getline(
            file,
            lastCrash
        );

        file.close();
    }

    std::cout
        << "CrashMon interface initialized\n";

    return true;
}


bool CrashMonInterface::recordCrash(
    int pid,
    int signal,
    const std::string& signalName
)
{
    crashCount++;

    std::time_t currentTime =
        std::time(nullptr);

    std::tm* localTime =
        std::localtime(&currentTime);

    std::ostringstream timestamp;

    timestamp
        << std::put_time(
            localTime,
            "%Y-%m-%d %H:%M:%S"
        );

    std::ostringstream information;

    information
        << timestamp.str()
        << " | PID: "
        << pid
        << " | Signal: "
        << signal
        << " ("
        << signalName
        << ")";

    lastCrash =
        information.str();


    std::ofstream file(
        stateFile
    );

    if (!file.is_open()) {

        std::cerr
            << "Failed to write CrashMon state\n";

        return false;
    }


    file
        << crashCount
        << "\n"
        << lastCrash
        << "\n";


    file.close();


    std::cout
        << "CrashMon recorded crash #"
        << crashCount
        << "\n";


    return true;
}


int CrashMonInterface::getCrashCount()
{
    return crashCount;
}


std::string CrashMonInterface::getLastCrash()
{
    return lastCrash;
}


void CrashMonInterface::shutdown()
{
    std::cout
        << "CrashMon interface shutdown\n";
}
