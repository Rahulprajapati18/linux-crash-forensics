# Testing, Integration & Improvement

## 1. Testing Objective

The objective of this stage is to verify the functional correctness, integration, reliability, and recovery behavior of the Linux Application Crash Forensics & Automatic Recovery Framework.

Testing focuses on the complete pipeline:

```text
Target Application
       |
       v
Process Monitoring
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
Crash Recording
       |
       v
Automatic Recovery
```

Both normal and intentionally crashing applications are used to verify the behavior of the framework.

---

## 2. Test Environment

The framework was tested in Ubuntu running under WSL2.

The main tools used during testing were:

* GCC/G++
* GDB
* Linux `/proc` filesystem
* Linux process management interfaces
* Core dumps
* Git

Core dumps were enabled using:

```bash
ulimit -c unlimited
```

---

## 3. Test Strategy

Testing is divided into the following categories:

1. Build testing
2. Unit-level module testing
3. Normal execution testing
4. Crash detection testing
5. Evidence collection testing
6. Core dump testing
7. GDB analysis testing
8. Report generation testing
9. Recovery testing
10. Persistent-state testing
11. Integration testing
12. Edge-case testing

---

## 4. Test Case Matrix

| Test ID | Test Case                      | Expected Result                   | Result |
| ------- | ------------------------------ | --------------------------------- | ------ |
| TC-01   | Compile complete framework     | Build succeeds without errors     | Passed |
| TC-02   | Run normal test directly       | Application exits normally        | Passed |
| TC-03   | Monitor normal application     | Exit code 0 detected              | Passed |
| TC-04   | Run crash test directly        | Application generates SIGSEGV     | Passed |
| TC-05   | Detect SIGSEGV through monitor | Crash correctly identified        | Passed |
| TC-06   | Collect `/proc` evidence       | Process information collected     | Passed |
| TC-07   | Locate core dump               | `core.<PID>` identified           | Passed |
| TC-08   | Run GDB analysis               | Backtrace and registers collected | Passed |
| TC-09   | Generate forensic report       | Timestamped report created        | Passed |
| TC-10   | Record crash event             | Crash count updated               | Passed |
| TC-11   | Restart crashed application    | Recovery process starts           | Passed |
| TC-12   | Limit recovery attempts        | Recovery stops after 3 attempts   | Passed |
| TC-13   | Verify persistent state        | Crash state remains stored        | Passed |
| TC-14   | Complete end-to-end flow       | All modules operate together      | Passed |

---

## 5. TC-01 — Framework Compilation

### Command

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

### Expected Result

The complete C++ framework should compile successfully.

### Observed Result

The command completed successfully with exit code:

```text
0
```

No compiler errors or warnings were produced.

**Status: Passed**

---

## 6. TC-02 — Normal Application

The normal test application is used to verify successful execution.

### Command

```bash
./normal_test
```

### Expected Result

The application should terminate normally.

### Observed Result

The application produced:

```text
Normal test application started
Application exiting normally
```

**Status: Passed**

---

## 7. TC-03 — Monitor Normal Application

### Command

```bash
./crash_monitor ./normal_test
```

### Expected Result

The monitor should identify normal process termination and report the exit code.

### Observed Result

```text
Process exited normally
Exit code: 0
```

**Status: Passed**

### Observation

Because `normal_test` exits very quickly, the process had already entered the zombie state when the monitor performed its one-second evidence collection.

Therefore:

```text
Process State   : Z (zombie)
Memory Usage    : Unavailable
Virtual Memory  : Unavailable
Command Line    :
```

This behavior is a limitation of the current prototype's fixed evidence-collection timing.

---

## 8. TC-04 — Crash Test Application

The crash test intentionally generates a segmentation fault.

### Command

```bash
./crash_test
```

The application waits three seconds and then executes:

```cpp
int* ptr = nullptr;
*ptr = 100;
```

### Expected Result

The process should terminate with `SIGSEGV`.

**Status: Passed**

---

## 9. TC-05 — Crash Detection

### Command

```bash
./crash_monitor ./crash_test
```

### Expected Result

The monitor should detect signal-based termination.

### Observed Result

```text
Signal : SIGSEGV - Segmentation Fault
```

Signal number:

```text
11
```

**Status: Passed**

---

## 10. TC-06 — Process Evidence Collection

The framework reads information from `/proc/<PID>/status` and `/proc/<PID>/cmdline`.

### Expected Evidence

* Process name
* Parent PID
* Process state
* Memory usage
* Virtual memory
* Thread count
* Command line

### Observed Result

Example:

```text
Process Name    : crash_test
Parent PID      : 50000
Process State   : S (sleeping)
Memory Usage    : 3996 kB
Virtual Memory  : 6896 kB
Threads         : 1
Command Line    : ./crash_test
```

**Status: Passed**

---

## 11. TC-07 — Core Dump Detection

Core dumps were enabled using:

```bash
ulimit -c unlimited
```

The framework searched for the core dump using the crashed process PID.

### Observed Result

For PID `50001`, the corresponding core dump was:

```text
core.50001
```

The monitor reported:

```text
Core   : core.50001
```

**Status: Passed**

---

## 12. TC-08 — GDB Crash Analysis

The GDB Analyzer automatically executes GDB against the executable and core dump.

The analysis includes:

```text
bt
info registers
```

### Expected Result

GDB should identify the crash location and execution context.

### Observed Result

The report contained:

```text
Program terminated with signal SIGSEGV, Segmentation fault.
#0 ... in main () at tests/crash_test.cpp:11
11          *ptr = 100;
```

Register information was also captured, including:

```text
rip
rsp
rbp
rax
rbx
rcx
rdx
```

**Status: Passed**

---

## 13. TC-09 — Forensic Report Generation

The framework generated a timestamped report:

```text
reports/crash_2026-09-30_22-30-53.txt
```

The report contained:

* Crash time
* Application
* PID
* Parent PID
* Crash signal
* Signal number
* Process state
* Memory information
* Thread count
* Command line
* GDB analysis
* Register information

**Status: Passed**

---

## 14. TC-10 — Crash State Recording

The CrashMon interface records crash events in:

```text
logs/crashmon_state.txt
```

During testing, the crash counter increased:

```text
1
2
3
```

The latest crash event included:

```text
PID: 50001
Signal: 11
SIGSEGV - Segmentation Fault
```

**Status: Passed**

---

## 15. TC-11 — Automatic Recovery

After detecting a crash, the Recovery Manager attempts to restart the target application.

### Configuration

```text
Maximum recovery attempts = 3
```

### Observed Result

```text
Recovery attempt 1 of 3
Restarted application crashed.

Recovery attempt 2 of 3
Restarted application crashed.

Recovery attempt 3 of 3
Restarted application crashed.
```

**Status: Passed**

---

## 16. TC-12 — Recovery Limit

The recovery manager must not restart an application indefinitely.

After the third failed restart, the framework reported:

```text
Maximum recovery attempts reached.
Automatic recovery stopped.
```

This demonstrates bounded recovery behavior.

**Status: Passed**

---

## 17. TC-13 — Persistent Crash State

Crash information is stored in:

```text
logs/crashmon_state.txt
```

The state survives execution of the monitoring program.

Example:

```text
3
2026-09-30 22:30:53 | PID: 50001 | Signal: 11 (SIGSEGV - Segmentation Fault)
```

**Status: Passed**

---

## 18. TC-14 — End-to-End Integration Test

The complete integrated test was performed using:

```bash
./crash_monitor ./crash_test
```

### Complete Observed Flow

```text
CrashMon initialization
        |
        v
Target process creation
        |
        v
Process evidence collection
        |
        v
Application crash
        |
        v
SIGSEGV detection
        |
        v
CrashMon event recording
        |
        v
Core dump detection
        |
        v
GDB analysis
        |
        v
Forensic report generation
        |
        v
Recovery attempt 1
        |
        v
Recovery attempt 2
        |
        v
Recovery attempt 3
        |
        v
Recovery limit reached
```

**Status: Passed**

---

## 19. Module Integration

The following modules were integrated into the main framework:

```text
main.cpp
   |
   +-- CrashDetector
   |
   +-- EvidenceCollector
   |
   +-- GDBAnalyzer
   |
   +-- CrashReport
   |
   +-- RecoveryManager
   |
   +-- CrashMonInterface
```

Each module performs a separate responsibility while the main monitor coordinates the overall workflow.

---

## 20. Failure and Edge Cases

### 20.1 Normal Process Exit

A normally terminating process should not trigger crash recovery.

**Observed:** The framework reported exit code `0`.

**Result:** Passed.

### 20.2 Very Short-Lived Process

A process that exits before evidence collection may cause `/proc` information to become unavailable.

**Observed:** `normal_test` produced a zombie state and unavailable memory fields.

**Result:** Known limitation.

### 20.3 Repeated Application Crash

An application that crashes after every restart could otherwise create an infinite recovery loop.

**Observed:** Recovery stopped after three attempts.

**Result:** Passed.

### 20.4 Missing Core Dump

If a core dump is unavailable, the framework handles the condition by reporting:

```text
Core dump not found
```

GDB analysis is skipped when there is no core file.

**Result:** Supported by implementation.

### 20.5 GDB Unavailable

The GDB analyzer checks whether the GDB process can be started. If the analysis command cannot be executed, an analysis failure message is returned.

---

## 21. Improvements Identified

Testing identified several possible improvements.

### Improvement 1 — Dynamic Evidence Collection

The current implementation waits for one second before collecting evidence.

A future implementation can periodically check whether the target is still running using:

```text
waitpid(..., WNOHANG)
```

This would improve handling of short-lived applications.

### Improvement 2 — Configurable Recovery Policy

The maximum recovery attempts are currently fixed at three.

A future version can allow configuration through a configuration file or command-line option.

### Improvement 3 — Improved Core-Dump Discovery

The current implementation searches for:

```text
core.<PID>
```

A future version can support configurable core-dump patterns and additional Linux core-dump locations.

### Improvement 4 — Structured Reports

The current reports are text files.

Future versions could additionally generate:

```text
JSON
CSV
SQLite
```

formats for automated analysis.

### Improvement 5 — Real Kernel Driver Integration

The current CrashMon functional interface is implemented in user space.

The next development phase can integrate the character-device driver when a suitable kernel build environment is available.

---

## 22. Test Results Summary

| Category               | Result |
| ---------------------- | ------ |
| Compilation            | Passed |
| Normal execution       | Passed |
| Crash detection        | Passed |
| Process evidence       | Passed |
| Core dump              | Passed |
| GDB analysis           | Passed |
| Report generation      | Passed |
| Crash recording        | Passed |
| Automatic recovery     | Passed |
| Recovery limiting      | Passed |
| Persistent state       | Passed |
| End-to-end integration | Passed |

The prototype successfully demonstrates the intended user-space crash-forensics and automatic-recovery workflow.

---

## 23. Conclusion

Testing confirms that the implemented framework can detect an application crash, collect Linux process evidence, locate a core dump, perform automated GDB analysis, generate a persistent forensic report, record the crash event, and attempt bounded automatic recovery.

The testing phase also identified practical limitations, particularly around very short-lived processes, core-dump availability, configurable recovery policies, and kernel-driver integration.

These findings provide the basis for the final implementation and presentation stage.
