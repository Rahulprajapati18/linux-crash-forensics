#ifndef GDB_ANALYZER_HPP
#define GDB_ANALYZER_HPP

#include <string>

class GDBAnalyzer {
public:
    static std::string analyze(
        const std::string& executable,
        const std::string& coreFile
    );
};

#endif
