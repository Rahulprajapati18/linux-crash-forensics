#ifndef EVIDENCE_COLLECTOR_HPP
#define EVIDENCE_COLLECTOR_HPP

#include <string>

class EvidenceCollector {
public:
    static std::string getProcessName(int pid);
    static std::string getParentPid(int pid);
    static std::string getProcessState(int pid);
    static std::string getMemoryUsage(int pid);
    static std::string getVirtualMemory(int pid);
    static std::string getThreadCount(int pid);
    static std::string getCommandLine(int pid);
};

#endif
