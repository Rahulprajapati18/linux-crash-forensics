# Linux Application Crash Forensics & Automatic Recovery Framework

## 1. System Overview

The Linux Application Crash Forensics & Automatic Recovery Framework is a software-based Linux monitoring and recovery system designed to detect application crashes, collect process-level evidence, analyze crash information, generate forensic reports, and automatically restart the failed application.

The framework is implemented primarily in C++ and uses Linux system interfaces such as `fork()`, `waitpid()`, `/proc`, core dumps, and GDB.

A character-device driver prototype named `crashmon` is also included to represent kernel-level communication. Due to the limitations of the WSL2 kernel development environment, the complete loadable kernel module could not be built. Therefore, the current functional integration uses a user-space `CrashMonInterface` as a prototype of the driver interface.

## 2. High-Level Architecture

```text
                    USER / ADMIN
                         |
                         v
                 +---------------+
                 |  C++ Monitor  |
                 |   / CLI       |
                 +-------+-------+
                         |
                         v
              +----------------------+
              | Crash Forensics      |
              | Engine               |
              +----------+-----------+
                         |
          +--------------+--------------+
          |              |              |
          v              v              v
   Crash Detector   Evidence       GDB Analyzer
                    Collector
          |              |              |
          |          /proc /sys          |
          +--------------+--------------+
                         |
                         v
                +----------------+
                | Crash Report   |
                | Generator      |
                +-------+--------+
                        |
                        v
                +----------------+
                | Recovery       |
                | Manager        |
                +-------+--------+
                        |
                        v
                 Restart Process


                         |
                         | User-space
                         | interface
                         v
                +----------------+
                | CrashMon       |
                | Interface      |
                +-------+--------+
                        |
                        v
                +----------------+
                | /dev/crashmon  |
                | Character      |
                | Driver         |
                | Prototype      |
                +-------+--------+
                        |
                        v
                 Linux Kernel
```

## 3. Major Components

### 3.1 Main Monitor

**File:** `src/main.cpp`

The main monitoring program coordinates all major framework components.

Responsibilities:

* Start the target application using `fork()` and `execl()`.
* Monitor the child process.
* Collect pre-crash process information.
* Wait for process termination using `waitpid()`.
* Determine whether the process terminated normally or because of a signal.
* Trigger crash analysis.
* Generate a forensic report.
* Start the recovery process.

The executable can monitor the default `./crash_test` application or another executable supplied as a command-line argument.

Example:

```bash
./crash_monitor ./crash_test
```

or:

```bash
./crash_monitor ./normal_test
```

## 4. Crash Detection Module

**Files:**

* `include/crash_detector.hpp`
* `src/crash_detector.cpp`

The Crash Detector determines whether the monitored process terminated because of a signal.

Linux process status macros are used:

```text
WIFSIGNALED(status)
WTERMSIG(status)
WIFEXITED(status)
WEXITSTATUS(status)
```

The framework currently recognizes signals including:

* SIGSEGV
* SIGABRT
* SIGFPE
* SIGILL
* SIGTERM
* SIGKILL

For a crash such as a segmentation fault, the framework identifies:

```text
SIGSEGV - Segmentation Fault
```

## 5. Evidence Collection Module

**Files:**

* `include/evidence_collector.hpp`
* `src/evidence_collector.cpp`

The Evidence Collector retrieves process information from the Linux `/proc` pseudo-filesystem.

Collected information includes:

* Process name
* Parent PID
* Process state
* Resident memory usage
* Virtual memory usage
* Thread count
* Command line

Example sources:

```text
/proc/<PID>/status
/proc/<PID>/cmdline
```

This information provides contextual evidence about the process around the time of failure.

## 6. Core Dump Collection

The framework uses Linux core dumps as crash evidence.

The test environment enables core dumps using:

```bash
ulimit -c unlimited
```

The WSL2 environment uses a core dump naming pattern similar to:

```text
core.<PID>
```

The framework searches for the corresponding core file using the crashed process PID.

Example:

```text
core.49638
```

Core dumps provide the process memory and execution state required for deeper post-crash analysis.

## 7. GDB Analysis Module

**Files:**

* `include/gdb_analyzer.hpp`
* `src/gdb_analyzer.cpp`

The GDB Analyzer performs automated post-mortem analysis of the executable and core dump.

The framework executes GDB in batch mode and collects:

```text
bt
info registers
```

The backtrace helps identify the call stack at the time of failure, while register information provides additional CPU execution context.

Example finding:

```text
Program terminated with signal SIGSEGV
#0 ... in main () at tests/crash_test.cpp:11
11    *ptr = 100;
```

The resulting GDB output is included in the forensic report.

## 8. Crash Report Generator

**Files:**

* `include/crash_report.hpp`
* `src/crash_report.cpp`

The Crash Report Generator creates a timestamped text report for every detected crash.

Reports are stored under:

```text
reports/
```

The report contains:

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
* GDB backtrace
* CPU register information

Example filename:

```text
crash_2026-09-30_22-15-56.txt
```

This creates persistent evidence that can be reviewed after the monitoring process terminates.

## 9. Recovery Manager

**Files:**

* `include/recovery_manager.hpp`
* `src/recovery_manager.cpp`

The Recovery Manager attempts to restart a crashed application.

The current prototype uses a bounded recovery policy:

```text
Maximum attempts = 3
```

Recovery flow:

```text
Application crashes
       |
       v
Recovery attempt 1
       |
       +---- succeeds ---> Stop recovery
       |
       +---- crashes ----> Attempt 2
```
