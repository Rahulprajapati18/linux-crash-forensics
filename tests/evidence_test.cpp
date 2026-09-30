#include <iostream>
#include <unistd.h>

#include "evidence_collector.hpp"

int main() {

    int pid = getpid();

    std::cout << "Current PID: " << pid << "\n\n";

    std::cout << "========== PROCESS STATUS ==========\n";

    std::cout << EvidenceCollector::getProcessStatus(pid);

    std::cout << "\n========== COMMAND LINE ==========\n";

    std::cout << EvidenceCollector::getCommandLine(pid) << "\n";

    return 0;
}
