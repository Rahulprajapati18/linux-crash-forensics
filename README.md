# Linux Application Crash Forensics & Automatic Recovery Framework

A C++-based Linux system monitoring and crash-analysis framework that detects application crashes, collects process-level evidence, analyzes core dumps using GDB, generates forensic reports, and attempts controlled automatic recovery.

---

## 📌 Project Overview

Applications running on Linux can terminate unexpectedly because of problems such as segmentation faults, illegal instructions, arithmetic exceptions, or abnormal termination.

When an application crashes, simply knowing that it stopped is often not enough. Developers and system administrators need to know:

- What caused the crash?
- Which signal terminated the process?
- Which process was affected?
- What was the process state before the crash?
- Where exactly did the failure occur?
- What was the CPU state at the time of failure?
- Can the application be safely restarted?

This project addresses these problems by providing an automated **crash detection, forensic analysis, reporting, and recovery pipeline**.

---

## 🎯 Problem Statement

Traditional application monitoring can detect that a process has stopped, but it may not provide enough information to understand the failure.

Manual investigation usually requires several separate tools and steps:

```text
Application Crash
       ↓
Find Core Dump
       ↓
Run GDB
       ↓
Analyze Stack Trace
       ↓
Inspect Registers
       ↓
Collect Process Information
       ↓
Create Report
       ↓
Restart Application
```

This project combines these activities into a single automated framework.

---

## 💡 Project Objective

The main objective is to build a Linux-based framework that can:

1. Monitor a running application.
2. Detect abnormal process termination.
3. Identify the crash signal.
4. Collect process information from `/proc`.
5. Locate the generated core dump.
6. Analyze the crash using GDB.
7. Generate a timestamped forensic report.
8. Attempt automatic application recovery.
9. Limit recovery attempts to prevent infinite restart loops.
10. Provide a foundation for future kernel-level crash monitoring.

---

## 🏗️ System Architecture

```text
                    USER / ADMIN
                         │
                         ▼
                  C++ CLI / Monitor
                         │
                         ▼
             Crash Forensics Engine
        ┌──────────────┬───────────────┐
        │              │               │
        ▼              ▼               ▼
 Crash Detector   Evidence Collector  GDB Analyzer
        │              │               │
        │           /proc /sys         │
        │              │               ▼
        │              │          Core Dump
        │              │               │
        └──────────────┴───────┬───────┘
                               ▼
                         Crash Report
                               │
                               ▼
                      Recovery Manager
                               │
                               ▼
                         Restart App
                               │
                               ▼
                     Controlled Recovery
```

### Future Kernel-Level Component

The project also contains a character-driver prototype intended to provide a future kernel/user-space interface:

```text
User Space
    │
    │ read / ioctl
    ▼
/dev/crashmon
    │
    ▼
Custom Character Driver
    │
    ▼
Linux Kernel
```

The current project uses a **user-space CrashMon interface** for functional testing. The kernel driver source is a prototype and is not currently claimed as a loaded/functional kernel module.

---

## 🔍 How the System Works

### 1. Start Application

The framework launches the target application using Linux process-management functions such as:

```text
fork()
exec()
waitpid()
```

---

### 2. Collect Pre-Crash Evidence

Before waiting for the application to finish, the framework collects information from Linux `/proc`.

Examples include:

- Process name
- Parent PID
- Process state
- Resident memory usage
- Virtual memory usage
- Thread count
- Command line

Example:

```text
========== PRE-CRASH EVIDENCE ==========

Process Name    : crash_test
Parent PID      : 12345
Process State   : S (sleeping)
Memory Usage    : 1892 kB
Virtual Memory  : 6548 kB
Threads         : 1
Command Line    : ./build/crash_test
```

---

### 3. Detect Crash

The framework waits for the child process using:

```cpp
waitpid()
```

It then checks the process status using Linux process-status macros such as:

```cpp
WIFSIGNALED()
WTERMSIG()
```

For example:

```text
SIGSEGV - Segmentation Fault
```

---

### 4. Locate Core Dump

When a process crashes, Linux can generate a **core dump**, which contains a snapshot of the process state at the time of failure.

The framework searches for the corresponding file:

```text
core.<PID>
```

Example:

```text
core.50596
```

---

### 5. Analyze Core Dump Using GDB

The core dump is passed to GDB together with the executable.

The framework automatically performs analysis such as:

```text
bt
info registers
```

This provides information including:

- Stack backtrace
- Crash location
- Function information
- CPU register state

Example:

```text
Program terminated with signal SIGSEGV,
Segmentation fault.

#0  main() at tests/crash_test.cpp:11

11      *ptr = 100;
```

---

### 6. Generate Forensic Report

The collected information is stored in a timestamped report:

```text
reports/crash_YYYY-MM-DD_HH-MM-SS.txt
```

A report contains information such as:

```text
Crash Signal
Signal Number
Application
PID
Process State
Memory Usage
Thread Count
Command Line
Core Dump
GDB Analysis
Register Information
```

This provides a persistent record that can be used for debugging and system analysis.

---

### 7. Automatic Recovery

After generating the report, the framework attempts to restart the application.

Recovery is intentionally bounded:

```text
Recovery attempt 1
Recovery attempt 2
Recovery attempt 3
        ↓
Maximum attempts reached
        ↓
Automatic recovery stopped
```

This prevents an application that continuously crashes from entering an infinite restart loop.

---

# 🧪 Demonstration

The project includes a deliberately crashing test application:

```cpp
int* ptr = nullptr;
*ptr = 100;
```

This produces a segmentation fault.

The framework detects:

```text
Signal : SIGSEGV - Segmentation Fault
```

Then:

```text
Core   : core.<PID>
```

GDB analyzes the core dump and identifies the failing source line.

Finally, a forensic report is generated and controlled recovery is attempted.

---

# 🛠️ Technologies Used

| Technology | Purpose |
|---|---|
| **C++17** | Core framework implementation |
| **Linux** | Target operating system |
| **CMake** | Build system |
| **GDB** | Core-dump and crash analysis |
| **Linux `/proc`** | Process evidence collection |
| **fork/exec/waitpid** | Process management |
| **Core Dumps** | Crash-state preservation |
| **Linux Signals** | Crash detection |
| **Character Driver (Prototype)** | Future kernel/user-space communication |
| **Git/GitHub** | Version control and project hosting |

---

# 📂 Project Structure

```text
linux-crash-forensics/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── src/
│   ├── main.cpp
│   ├── crash_detector.cpp
│   ├── evidence_collector.cpp
│   ├── crash_report.cpp
│   ├── gdb_analyzer.cpp
│   ├── recovery_manager.cpp
│   └── crashmon_interface.cpp
│
├── include/
│   ├── crash_detector.hpp
│   ├── evidence_collector.hpp
│   ├── crash_report.hpp
│   ├── gdb_analyzer.hpp
│   ├── recovery_manager.hpp
│   └── crashmon_interface.hpp
│
├── tests/
│   ├── crash_test.cpp
│   ├── normal_test.cpp
│   └── evidence_test.cpp
│
├── driver/
│   └── crashmon.c
│
├── docs/
│   ├── system_architecture.md
│   ├── uml_design.md
│   ├── initial_prototype.md
│   ├── testing_and_integration.md
│   └── git_workflow.md
│
├── logs/
│
├── reports/
│
└── build/
```

---

# 🚀 Installation

### 1. Clone the Repository

```bash
git clone https://github.com/Rahulprajapati18/linux-crash-forensics.git
cd linux-crash-forensics
```

### 2. Install Dependencies

Ubuntu/Debian:

```bash
sudo apt update
sudo apt install g++ cmake gdb git
```

### 3. Enable Core Dumps

```bash
ulimit -c unlimited
```

Check the core dump configuration:

```bash
cat /proc/sys/kernel/core_pattern
```

---

# 🔨 Build the Project

Create the build directory:

```bash
mkdir -p build
cd build
```

Configure:

```bash
cmake ..
```

Compile:

```bash
make -j$(nproc)
```

The main executable will be:

```text
build/crash_monitor
```

---

# ▶️ Running the Project

## Test 1 — Normal Application

Run:

```bash
./build/crash_monitor ./build/normal_test
```

Expected behavior:

```text
Process exited normally
Exit code: 0
```

---

## Test 2 — Crash Application

Run:

```bash
./build/crash_monitor ./build/crash_test
```

The application intentionally produces a segmentation fault.

The framework should:

```text
1. Start application
2. Collect process evidence
3. Detect SIGSEGV
4. Locate core dump
5. Run GDB analysis
6. Generate crash report
7. Attempt recovery
8. Stop after maximum attempts
```

---

# 🔎 Check Generated Reports

List reports:

```bash
ls reports/
```

Open a report:

```bash
cat reports/crash_*.txt
```

A report contains the crash information and GDB analysis.

---

# 🧪 Evidence Collector Test

Run:

```bash
./build/evidence_test
```

This verifies process information collection through `/proc`.

---

# 🐛 Manual GDB Analysis

The core dump can also be analyzed manually:

```bash
gdb ./build/crash_test core.<PID>
```

Inside GDB:

```text
bt
```

View registers:

```text
info registers
```

Exit:

```text
quit
```

The framework automates this analysis so that manual GDB interaction is not required for every crash.

---

# 📊 Supported Crash Signals

The framework currently handles signals including:

| Signal | Meaning |
|---|---|
| `SIGSEGV` | Segmentation Fault |
| `SIGABRT` | Aborted |
| `SIGFPE` | Arithmetic Exception |
| `SIGILL` | Illegal Instruction |
| `SIGTERM` | Terminated |
| `SIGKILL` | Killed |

---

# 🔐 Recovery Safety

Automatic recovery is intentionally limited.

The current implementation allows a maximum of:

```text
3 recovery attempts
```

This design prevents:

```text
Crash
 ↓
Restart
 ↓
Crash
 ↓
Restart
 ↓
Crash
 ↓
Restart
 ↓
∞
```

Instead:

```text
Crash
 ↓
Restart × 3
 ↓
Stop
 ↓
Keep forensic evidence
```

This makes the recovery mechanism safer for applications that repeatedly fail.

---

# ⚠️ Current Limitations

### 1. Kernel Driver

The `driver/crashmon.c` component is currently a **character-driver prototype**.

The current functional monitoring pipeline uses the user-space CrashMon interface. A loaded `/dev/crashmon` kernel device is not currently part of the tested runtime.

### 2. Core Dump Configuration

Core dumps must be enabled and correctly configured by the Linux environment.

### 3. Pre-Crash Evidence Timing

The framework currently collects evidence shortly after starting the process. Very short-lived applications may exit before all `/proc` information can be collected.

### 4. Recovery Policy

The current recovery mechanism uses a fixed maximum number of restart attempts rather than application-specific recovery policies.

---

# 🔮 Future Enhancements

Possible future improvements include:

- Fully integrate the character device driver with the framework
- Implement `/dev/crashmon` kernel communication
- Add `ioctl()`-based kernel/user-space communication
- Add configurable recovery policies
- Add crash severity classification
- Add persistent crash history
- Add systemd service integration
- Add resource monitoring before crashes
- Add a graphical dashboard
- Add email/notification support
- Support monitoring multiple applications simultaneously
- Add more advanced GDB analysis such as thread backtraces and shared-library information

---

# 🎓 Learning Outcomes

This project demonstrates practical knowledge of:

- Linux process management
- C++ system programming
- Process creation using `fork()`
- Program execution using `exec()`
- Process synchronization using `waitpid()`
- Linux signals
- `/proc` filesystem
- Core dumps
- GDB debugging
- Stack backtraces
- CPU registers
- File handling
- CMake
- Linux character-driver concepts
- Kernel/user-space communication concepts
- Automatic recovery mechanisms
- Git and GitHub workflow

---

# 📌 Project Highlights

```text
✓ Linux-based crash monitoring
✓ Automatic crash detection
✓ Signal identification
✓ Pre-crash process evidence
✓ Core dump detection
✓ Automated GDB analysis
✓ Stack trace collection
✓ CPU register collection
✓ Timestamped forensic reports
✓ Controlled automatic recovery
✓ C++17 implementation
✓ CMake build system
✓ Character-driver prototype
```

---

# 👨‍💻 Author

**Rahul Prajapati**

B.Tech — Computer Science & Engineering

GitHub:  
https://github.com/Rahulprajapati18

---

# 📄 License

This project is developed for educational, research, and system-programming purposes.
