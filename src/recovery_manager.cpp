#include "recovery_manager.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

bool RecoveryManager::restart(
    const std::string& executable,
    int maxAttempts
) {

    for (int attempt = 1;
         attempt <= maxAttempts;
         ++attempt) {

        std::cout
            << "\n========== RECOVERY ==========\n";

        std::cout
            << "Recovery attempt "
            << attempt
            << " of "
            << maxAttempts
            << "\n";

        pid_t pid = fork();

        if (pid < 0) {

            std::cerr
                << "Failed to create recovery process\n";

            return false;
        }

        if (pid == 0) {

            execl(
                executable.c_str(),
                executable.c_str(),
                nullptr
            );

            std::cerr
                << "Failed to restart application\n";

            _exit(1);
        }

        std::cout
            << "Restarted application with PID: "
            << pid
            << "\n";

        int status;

        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {

            std::cout
                << "Application exited normally.\n";

            std::cout
                << "==============================\n";

            return true;
        }

        if (WIFSIGNALED(status)) {

            std::cout
                << "Restarted application crashed.\n";
        }
    }

    std::cout
        << "Maximum recovery attempts reached.\n";

    std::cout
        << "Automatic recovery stopped.\n";

    std::cout
        << "==============================\n";

    return false;
}
