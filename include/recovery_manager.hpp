#ifndef RECOVERY_MANAGER_HPP
#define RECOVERY_MANAGER_HPP

#include <string>

class RecoveryManager {
public:

    static bool restart(
        const std::string& executable,
        int maxAttempts
    );
};

#endif
