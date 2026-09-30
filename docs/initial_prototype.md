# Initial Implementation & Prototype

## 1. Objective

The objective of the initial prototype is to implement the core Linux crash-forensics workflow and demonstrate that the framework can:

1. Start and monitor a Linux application.
2. Detect abnormal process termination.
3. Collect process-level evidence from `/proc`.
4. Detect and locate a core dump.
5. Perform automated GDB analysis.
6. Generate a persistent forensic report.
7. Record crash information.
8. Automatically restart a failed application using a bounded recovery policy.

The prototype is implemented using C++ and Linux system interfaces.

---

## 2. Implementation Environment

The prototype was developed and tested in an Ubuntu environment running under WSL2.

Main technologies used:

| Technology               | Purpose                         |
| ------------------------ | ------------------------------- |
| C++                      | Main framework implementation   |
| Linux system calls       | Process creation and monitoring |
| `/proc` filesystem       | Process evidence collection     |
| GDB                      | Core-dump analysis              |
| Core dumps               | Post-crash evidence             |
| Linux signals            | Crash detection                 |
| Git                      | Version control                 |
| Linux character-driver C | Driver prototype                |

---

## 3. Project Structure

```text
linux-crash-forensics/
├── .gitignore
├── driver/
│   ├── Makefile
│   └── crashmon.c
├── include/
│   ├── crash_detector.hpp
│   ├── crash_report.hpp
│   ├── crashmon_interface.hpp
│   ├── evidence_collector.hpp
│   ├── gdb_analyzer.hpp
│   └── recovery_manager.hpp
├── src/
│   ├── crash_detector.cpp
│   ├── crash_report.cpp
│   ├── crashmon_interface.cpp
│   ├── evidence_collector.cpp
│   ├── gdb_analyzer.cpp
│   ├── main.cpp
│   └── recovery_manager.cpp
├── tests/
│   ├── crash_test.cpp
│   ├── evidence_test.cpp
│   └── normal_test.cpp
└── docs/
    ├── system_architecture.md
    ├── uml_design.md
    ├── git_workflow.md
    └── initial_prototype.md
```

---

## 4. Crash Test Application

**File:**

```text
tests/crash_test.cpp
```

The crash test application intentionally generates a segmentation fault.

The application waits for three seconds and then dereferences a null pointer:

```cpp
int* ptr = nullptr;
*ptr = 100;
```

The application is compiled with debugging information:

```bash
g++ -g tests/crash_test.cpp -o crash_test
```

The debugging symbols are important because they allow GDB to associate the crash with the relevant source-code location.

---

## 5. Normal Test Application

**File:**

```text
tests/normal_test.cpp
```

The normal test application exits successfully without generating a crash.

It is used to verify that the framework correctly distinguishes between:

```text
Normal process termination
```

and:

```text
Signal-based process termination
```

Compilation:

```bash
g++ -g tests/normal_test.cpp -o normal_test
```

---

## 6. Crash Detection Implementation

**Files:**

```text
include/crash_detector.hpp
src/crash_detector.cpp
```

The crash detector analyzes the status returned by `waitpid()`.

The implementation uses Linux/POSIX process-status macros:

```text
WIFSIGNALED(status)
WTERMSIG(status)
WIFEXITED(status)
WEXITSTATUS(status)
```

If `WIFSIGNALED(status)` is true, the process terminated because of a signal.

The prototype converts recognized signal numbers into readable descriptions.

Examples:

```text
SIGSEGV - Segmentation Fault
SIGABRT - Aborted
SIGFPE - Arithmetic Exception
SIGILL - Illegal Instruction
SIGTERM - Terminated
SIGKILL - Killed
```

---

## 7. Process Evidence Collection

**Files:**

```text
include/evidence_collector.hpp
src/evidence_collector.cpp
```

The Evidence Collector reads process information from the Linux `/proc` filesystem.

The following information is collected:

* Process name
* Parent PID
* Process state
* Resident memory usage
* Virtual memory usage
* Thread count
* Command line

Primary sources:

```text
/proc/<PID>/status
/proc/<PID>/cmdline
```

Example evidence collected during a crash test:

```text
Process Name    : crash_test
Parent PID      : 49637
Process State   : S (sleeping)
Memory Usage    : 4004 kB
Virtual Memory  : 6896 kB
Threads         : 1
Command Line    : ./crash_test
```

---

## 8. Core Dump Configuration

Core dumps were enabled during testing using:

```bash
ulimit -c unlimited
```

The test environment used:

```text
core
```

as the configured core-pattern base, while the resulting files were observed with PID-specific names such as:

```text
core.49638
```

The framework searches for the core file corresponding to the monitored process PID.

---

## 9. GDB Analyzer

**Files:**

```text
include/gdb_analyzer.hpp
src/gdb_analyzer.cpp
```

The GDB Analyzer performs automated post-mortem debugging.

GDB is executed in batch mode with commands including:

```text
bt
info registers
```

The `bt` command obtains the call stack.

The `info registers` command collects CPU register information.

The resulting output is stored as part of the crash report.

A successful analysis identified the intentional crash at:

```text
tests/crash_test.cpp:11
```

with:

```text
*ptr = 100;
```

as the crashing statement.

---

## 10. Crash Report Generation

**Files:**

```text
include/crash_report.hpp
src/crash_report.cpp
```

The Crash Report Generator creates a timestamped text file under:

```text
reports/
```

The generated report contains:

* Crash timestamp
* Application name
* PID
* Parent PID
* Crash signal
* Signal number
* Process state
* Memory usage
* Virtual memory
* Thread count
* Command line
* GDB analysis
* Register information

Example:

```text
reports/crash_2026-09-30_22-15-56.txt
```

The report provides persistent evidence that can be inspected after the monitoring process has completed.

---

## 11. Automatic Recovery

**Files:**

```text
include/recovery_manager.hpp
src/recovery_manager.cpp
```

The Recovery Manager automatically attempts to restart a crashed application.

The prototype uses a maximum of three recovery attempts.

Example flow:

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

The bounded retry mechanism prevents an application that continuously crashes from being restarted indefinitely.

During testing, the intentionally crashing application produced:

```text
Recovery attempt 1 of 3
Restarted application crashed.

Recovery attempt 2 of 3
Restarted application crashed.

Recovery attempt 3 of 3
Restarted application crashed.

Maximum recovery attempts reached.
Automatic recovery stopped.
```

---

## 12. CrashMon Interface

**Files:**

```text
include/crashmon_interface.hpp
src/crashmon_interface.cpp
```

The CrashMon interface provides a user-space abstraction for crash-event recording.

It supports:

* Initialization
* Crash recording
* Crash count
* Last crash information
* Shutdown

Crash state is persisted in:

```text
logs/crashmon_state.txt
```

Example:

```text
2
2026-09-30 22:15:55 | PID: 49638 | Signal: 11 (SIGSEGV - Segmentation Fault)
```

This allows crash information to persist between executions of the monitor.

---

## 13. Character Driver Prototype

**Files:**

```text
driver/crashmon.c
driver/Makefile
```

A Linux character-device driver prototype named `crashmon` was prepared.

The driver contains basic character-device operations:

```text
open()
read()
write()
module_init()
module_exit()
```

The intended device path is:

```text
/dev/crashmon
```

### WSL2 Limitation

The development environment is WSL2.

The matching WSL kernel source was prepared and the driver source successfully reached the external-module compilation stage. However, creation of a loadable `.ko` module requires complete kernel build artifacts, including `Module.symvers`.

Building the complete WSL kernel was not practical in the development environment.

Therefore, the current prototype uses:

```text
crashmon.c
     |
     | Character-driver prototype
     |
     v

CrashMonInterface
     |
     | Functional user-space integration
     v

Crash Forensics Framework
```

The project does not claim that the `/dev/crashmon` kernel module is currently loaded.

---

## 14. Main Monitoring Workflow

The main application is implemented in:

```text
src/main.cpp
```

The monitor accepts an optional executable argument.

Default:

```bash
./crash_monitor
```

This monitors:

```text
./crash_test
```

A different executable can be supplied:

```bash
./crash_monitor ./normal_test
```

The main execution flow is:

```text
Initialize CrashMon
       |
       v
Select Target Executable
       |
       v
fork()
       |
       v
Child executes target
       |
       v
Parent collects evidence
       |
       v
waitpid()
       |
       v
Analyze process status
       |
       +----------------------+
       |                      |
       v                      v
Normal Exit              Crash
       |                      |
       v                      v
Exit Code              Record Crash
                              |
                              v
                       Find Core Dump
                              |
                              v
                         GDB Analysis
                              |
                              v
                       Generate Report
                              |
                              v
                       Restart Process
```

---

## 15. Prototype Build

The complete user-space prototype can be compiled using:

```bash
g++ -Wall -Wextra -Iinclude \
src/main.cpp \
src/crash_detector.cpp \
src/evidence_collector.cpp \
src/crash_report.cpp \
src/gdb_analyzer.cpp \
src/recovery_manager.cpp \
src/crashmon_interface.cpp \
-o crash_monitor
```

The build completed successfully without compiler errors.

---

## 16. Prototype Execution

### Normal Application

```bash
./crash_monitor ./normal_test
```

Expected behavior:

```text
Normal test application started
Application exiting normally
Process exited normally
Exit code: 0
```

### Crashing Application

```bash
./crash_monitor ./crash_test
```

Expected behavior:

```text
CrashMon interface initialized
Monitoring process PID: <PID>
PRE-CRASH EVIDENCE
...
CrashMon recorded crash
Running GDB crash analysis
Crash report generated successfully
RECOVERY
...
CRASH DETECTED
Signal: SIGSEGV - Segmentation Fault
Core: core.<PID>
```

---

## 17. Prototype Verification

The following tests were completed.

### Test 1 — Normal Execution

The normal test application exited with status code `0`.

**Result:** Passed.

### Test 2 — Normal Application Through Monitor

The monitor successfully started and monitored `normal_test`.

The framework correctly identified normal process termination and reported:

```text
Exit code: 0
```

**Result:** Passed.

### Test 3 — Crash Detection

The monitor successfully detected the segmentation fault generated by `crash_test`.

Detected signal:

```text
SIGSEGV - Segmentation Fault
```

**Result:** Passed.

### Test 4 — Evidence Collection

The framework collected process information from `/proc`.

Collected fields included:

```text
Name
PPid
State
VmRSS
VmSize
Threads
Command line
```

**Result:** Passed.

### Test 5 — GDB Analysis

The framework automatically analyzed the generated core dump.

GDB identified the crash location in:

```text
tests/crash_test.cpp:11
```

**Result:** Passed.

### Test 6 — Crash Report

A timestamped forensic report was generated containing process information and GDB analysis.

**Result:** Passed.

### Test 7 — Automatic Recovery

The recovery manager restarted the intentionally crashing application three times and then stopped after reaching the configured maximum.

**Result:** Passed.

### Test 8 — Persistent Crash State

Crash count and the most recent crash event were persisted in:

```text
logs/crashmon_state.txt
```

**Result:** Passed.

---

## 18. Current Prototype Limitations

The current prototype has the following limitations:

1. The character driver is a source-level prototype and is not currently loaded as a kernel module in WSL2.
2. Core-dump availability depends on Linux/WSL core-dump configuration.
3. Very short-lived applications may exit before the monitor's pre-crash evidence collection point, causing some `/proc` fields to become unavailable.
4. Recovery currently uses a fixed maximum of three attempts.
5. GDB analysis depends on GDB being installed and accessible.
6. The framework currently generates text reports rather than a structured database or web dashboard.

These limitations provide clear opportunities for future improvements.

---

## 19. Prototype Achievement

The initial prototype successfully demonstrates the main user-space crash-forensics pipeline:

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
Core Dump
     |
     v
GDB Analysis
     |
     v
Forensic Report
     |
     v
Automatic Recovery
```

The implementation establishes the core foundation required for further testing, integration, and final implementation.
