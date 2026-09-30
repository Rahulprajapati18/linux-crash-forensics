# Linux Application Crash Forensics & Automatic Recovery Framework

A Linux-based software framework for detecting application crashes, collecting process evidence, analyzing core dumps with GDB, generating forensic reports, and performing bounded automatic recovery.

## 1. Project Overview

Application crashes can result in loss of service and make debugging difficult when useful runtime evidence is not preserved.

This project provides an automated crash-forensics workflow that monitors a Linux application and performs the following operations when a crash occurs:

```text
Application
     |
     v
Crash Detection
     |
     v
Evidence Collection
     |
     v
Core Dump Detection
     |
     v
GDB Analysis
     |
     v
Forensic Report
     |
     v
Crash Recording
     |
     v
Automatic Recovery
```

The framework is implemented primarily in **C++** and uses Linux process-management interfaces, the `/proc` filesystem, core dumps, and GDB.

A Linux character-device driver prototype named `crashmon` is also included for future kernel-level integration.

---

## 2. Features

* Linux application process monitoring
* Crash detection using process status and signals
* SIGSEGV, SIGABRT, SIGFPE, SIGILL and other signal identification
* `/proc`-based process evidence collection
* Core dump detection
* Automated GDB post-mortem analysis
* Backtrace collection
* CPU register collection
* Timestamped forensic reports
* Persistent crash-event recording
* Bounded automatic application recovery
* Configurable target executable through command-line argument
* CMake-based build system
* Git version control
* Character-device driver prototype

---

## 3. Technologies Used

| Technology       | Purpose                    |
| ---------------- | -------------------------- |
| C++17            | Main framework             |
| Linux/POSIX APIs | Process monitoring         |
| `/proc`          | Process evidence           |
| GDB              | Crash analysis             |
| Core dumps       | Post-crash evidence        |
| C                | Character-driver prototype |
| CMake            | Build system               |
| Git              | Version control            |
| Ubuntu/WSL2      | Development environment    |

---

## 4. Architecture

```text
                    USER / ADMIN
                         |
                         v
                 +---------------+
                 |  C++ Monitor  |
                 +-------+-------+
                         |
                         v
              +----------------------+
              | Crash Forensics      |
              | Engine               |
              +----------+-----------+
                         |
        +----------------+----------------+
        |                |                |
        v                v                v
 Crash Detector   Evidence Collector   GDB Analyzer
        |                |                |
        |              /proc              |
        +----------------+----------------+
                         |
                         v
                  Crash Report
                         |
                         v
                 Recovery Manager
                         |
                         v
                    Restart App

                         |
                         v
                 CrashMon Interface
                         |
                         v
                  Driver Prototype
                         |
                         v
                    Linux Kernel
```

Detailed architecture and UML documentation are available in:

```text
docs/system_architecture.md
docs/uml_design.md
```

---

## 5. Project Structure

```text
linux-crash-forensics/
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── driver/
│   ├── Makefile
│   └── crashmon.c
│
├── include/
│   ├── crash_detector.hpp
│   ├── crash_report.hpp
│   ├── crashmon_interface.hpp
│   ├── evidence_collector.hpp
│   ├── gdb_analyzer.hpp
│   └── recovery_manager.hpp
│
├── src/
│   ├── crash_detector.cpp
│   ├── crash_report.cpp
│   ├── crashmon_interface.cpp
│   ├── evidence_collector.cpp
│   ├── gdb_analyzer.cpp
│   ├── main.cpp
│   └── recovery_manager.cpp
│
├── tests/
│   ├── crash_test.cpp
│   ├── evidence_test.cpp
│   └── normal_test.cpp
│
└── docs/
    ├── system_architecture.md
    ├── uml_design.md
    ├── git_workflow.md
    ├── initial_prototype.md
    └── testing_and_integration.md
```

---

## 6. Requirements

The following tools are required:

* C++ compiler
* CMake
* GDB
* Linux/WSL2 environment

For Ubuntu:

```bash
sudo apt update
sudo apt install build-essential cmake gdb
```

Enable core dumps for testing:

```bash
ulimit -c unlimited
```

---

## 7. Build Using CMake

Clone or enter the project directory:

```bash
cd ~/linux-crash-forensics
```

Configure:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build -j$(nproc)
```

The following executables are generated:

```text
build/crash_monitor
build/crash_test
build/normal_test
build/evidence_test
```

The `build/` directory is ignored by Git.

---

## 8. Running the Tests

### 8.1 Evidence Collector Test

```bash
./build/evidence_test
```

This verifies process-state and command-line information retrieval through `/proc`.

### 8.2 Normal Application Test

```bash
./build/normal_test
```

The application should exit normally.

### 8.3 Crash Test Application

```bash
./build/crash_test
```

This application intentionally generates a segmentation fault.

---

## 9. Running the Crash Monitor

### Monitor the default crash test

```bash
./build/crash_monitor
```

The default target is:

```text
./crash_test
```

### Monitor a specific executable

```bash
./build/crash_monitor ./build/crash_test
```

### Monitor the normal application

```bash
./build/crash_monitor ./build/normal_test
```

---

## 10. Crash Processing

When the monitored application crashes, the framework performs:

### Step 1 — Detect Crash

The framework checks the process termination status using Linux process-status macros.

### Step 2 — Identify Signal

For example:

```text
SIGSEGV - Segmentation Fault
```

### Step 3 — Record Crash

The CrashMon interface records the crash count and latest crash event.

### Step 4 — Locate Core Dump

The framework searches for the corresponding PID-based core dump.

Example:

```text
core.50596
```

### Step 5 — Analyze With GDB

GDB collects:

```text
bt
info registers
```

### Step 6 — Generate Report

A timestamped report is created under:

```text
reports/
```

### Step 7 — Recover

The Recovery Manager attempts to restart the application.

The current maximum is:

```text
3 attempts
```

After three failed attempts, recovery stops.

---

## 11. Example Crash Report

Example report:

```text
reports/crash_2026-09-30_22-39-54.txt
```

The report contains information such as:

```text
Crash Signal     : SIGSEGV - Segmentation Fault
Signal Number    : 11
Application      : crash_test
PID              : 50596
Memory Usage     : 3984 kB
Virtual Memory   : 6896 kB
Threads          : 1
Command Line     : ./build/crash_test
```

GDB identified the crash at:

```text
tests/crash_test.cpp:11
```

The faulting statement was:

```cpp
*ptr = 100;
```

The report also contains CPU register information.

---

## 12. Automatic Recovery

The recovery manager uses a bounded retry policy.

```text
Crash
  |
  v
Attempt 1
  |
  +-- Crash --> Attempt 2
                  |
                  +-- Crash --> Attempt 3
                                  |
                                  +-- Crash
                                      |
                                      v
                              Stop Recovery
```

This prevents an application that continuously crashes from being restarted indefinitely.

---

## 13. CrashMon Character Driver

The project contains a character-device driver prototype:

```text
driver/crashmon.c
```

The intended device is:

```text
/dev/crashmon
```

The prototype implements basic:

```text
open()
read()
write()
module_init()
module_exit()
```

### Current WSL2 Limitation

The project was developed under WSL2. The driver source was prepared and compiled through the external-module compilation stage, but producing a loadable `.ko` module requires complete kernel build artifacts such as `Module.symvers`.

The complete WSL kernel build was not used for the final functional demonstration.

Therefore, the current project should be understood as:

```text
Character Driver Prototype
          +
User-Space CrashMon Interface
          +
Functional Crash-Forensics Framework
```

The project does not claim that `/dev/crashmon` is currently loaded and running as a kernel module.

---

## 14. Testing Summary

The framework was tested using both normal and intentionally crashing applications.

| Test                               | Result |
| ---------------------------------- | ------ |
| CMake configuration                | Passed |
| CMake compilation                  | Passed |
| Evidence collector                 | Passed |
| Normal application                 | Passed |
| Normal application through monitor | Passed |
| Crash detection                    | Passed |
| SIGSEGV identification             | Passed |
| `/proc` evidence collection        | Passed |
| Core dump detection                | Passed |
| GDB analysis                       | Passed |
| Source-line identification         | Passed |
| Forensic report generation         | Passed |
| Persistent crash recording         | Passed |
| Automatic recovery                 | Passed |
| Recovery limit                     | Passed |
| End-to-end integration             | Passed |

---

## 15. Known Limitations

1. Very short-lived processes can exit before the current pre-crash evidence collection point.
2. Core-dump generation depends on system configuration.
3. GDB must be installed for automated analysis.
4. Recovery currently uses a maximum of three attempts.
5. The character driver is currently a prototype and is not loaded in the WSL2 test environment.
6. Reports are currently stored as text files.

---

## 16. Future Enhancements

Potential future improvements include:

* Dynamic process-state monitoring using `waitpid(..., WNOHANG)`
* Configurable recovery policies
* More sophisticated crash classification
* JSON/SQLite report storage
* Centralized crash history
* Dashboard for crash statistics
* Real kernel character-device integration
* Additional kernel-level crash-event communication
* Systemd service integration
* Multi-process monitoring

---

## 17. Documentation

Additional project documentation:

```text
docs/system_architecture.md
docs/uml_design.md
docs/git_workflow.md
docs/initial_prototype.md
docs/testing_and_integration.md
```

These documents cover the system architecture, UML design, Git workflow, prototype implementation, testing, integration, and identified improvements.

---

## 18. Git Version History

The project uses Git for version control.

Major commits include:

```text
Initial implementation of Linux crash forensics framework
Add system architecture and UML design documentation
Document initial prototype implementation
Add testing and integration documentation
Finalize build system and runtime integration
```

Check history with:

```bash
git log --oneline
```

Check repository status with:

```bash
git status
```

A clean working tree is expected before final submission.

---

## 19. Conclusion

The Linux Application Crash Forensics & Automatic Recovery Framework demonstrates an integrated Linux software-monitoring workflow.

The final prototype can detect application crashes, collect process-level evidence, locate core dumps, perform automated GDB analysis, generate forensic reports, persist crash information, and perform bounded automatic recovery.

The project also establishes a foundation for future kernel-level integration through the included `crashmon` character-driver prototype.
