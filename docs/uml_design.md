# UML Design — Linux Application Crash Forensics & Automatic Recovery Framework

## 1. Use Case Diagram

```text
                    +--------------------------------------+
                    | Linux Crash Forensics Framework      |
                    |                                      |
 Administrator ---->| Start Monitoring                     |
                    |                                      |
 Administrator ---->| Select Target Application            |
                    |                                      |
                    | Detect Application Crash              |
                    |          |                           |
                    |          v                           |
                    | Collect Process Evidence              |
                    |          |                           |
                    |          v                           |
                    | Analyze Core Dump using GDB            |
                    |          |                           |
                    |          v                           |
                    | Generate Forensic Report               |
                    |          |                           |
                    |          v                           |
                    | Record Crash Information               |
                    |          |                           |
                    |          v                           |
                    | Restart Failed Application             |
                    |          |                           |
                    |          v                           |
                    | Limit Recovery Attempts                |
                    +--------------------------------------+
```

### Primary Actor

**Administrator / User**

The administrator starts the monitoring framework, selects an application to monitor, and can inspect generated forensic reports.

### Main Use Cases

* Start monitoring
* Select target application
* Detect crash
* Collect process evidence
* Analyze core dump
* Generate crash report
* Record crash information
* Restart application
* Limit recovery attempts

---

## 2. Class Diagram

```text
+-------------------------+
|       CrashDetector     |
+-------------------------+
| + crashed(status)       |
| + getSignalName(signal) |
+-------------------------+

             |
             | uses
             v

+-------------------------+
|    EvidenceCollector    |
+-------------------------+
| + getProcessName(pid)   |
| + getParentPid(pid)     |
| + getProcessState(pid)  |
| + getMemoryUsage(pid)   |
| + getVirtualMemory(pid) |
| + getThreadCount(pid)   |
| + getCommandLine(pid)   |
+-------------------------+

             |
             | data
             v

+-------------------------+
|      CrashReport        |
+-------------------------+
| + generate(...)         |
+-------------------------+

             ^
             |
             | receives GDB output
             |
+-------------------------+
|      GDBAnalyzer        |
+-------------------------+
| + analyze(exe, core)    |
+-------------------------+

+-------------------------+
|    RecoveryManager      |
+-------------------------+
| + restart(exe, attempts)|
+-------------------------+

+-------------------------+
|   CrashMonInterface     |
+-------------------------+
| + initialize()          |
| + recordCrash(...)      |
| + getCrashCount()       |
| + getLastCrash()        |
| + shutdown()            |
+-------------------------+

                    |
                    | intended interface
                    v

+-------------------------+
|      crashmon Driver    |
+-------------------------+
| open()                  |
| read()                  |
| write()                 |
| init()                  |
| exit()                  |
+-------------------------+
```

---

## 3. Sequence Diagram — Normal Application

```text
User       Monitor       Target Application
 |            |                  |
 | Start      |                  |
 |----------->|                  |
 |            | fork()           |
 |            |----------------->|
 |            |                  |
 |            | collect evidence |
 |            |----------------->|
 |            |                  |
 |            | waitpid()        |
 |            |----------------->|
 |            |                  |
 |            |<-----------------|
 |            | normal exit      |
 |            |                  |
 |<-----------| Exit code 0      |
```

### Normal Flow

1. User starts the monitoring application.
2. Monitor creates the target process.
3. Monitor collects process information.
4. Monitor waits for termination.
5. Target exits normally.
6. Monitor reports the exit code.

---

## 4. Sequence Diagram — Crash Detection and Forensics

```text
Monitor       Target       CrashDetector
   |             |               |
   | fork()      |               |
   |-----------> |               |
   |             |               |
   | waitpid()   |               |
   |-----------> |               |
   |             X Crash         |
   |<------------|               |
   |             |               |
   |---------------------------> |
   |       Check status          |
   |<--------------------------- |
   |       Crash detected        |
```

After crash detection:

```text
CrashDetector
      |
      v
EvidenceCollector
      |
      v
Find Core Dump
      |
      v
GDBAnalyzer
      |
      v
CrashReport
      |
      v
RecoveryManager
```

---

## 5. Sequence Diagram — Automatic Recovery

```text
Monitor       RecoveryManager       Application
   |                 |                   |
   | restart()       |                   |
   |---------------->|                   |
   |                 | fork()            |
   |                 |------------------>|
   |                 |                   |
   |                 |<------------------|
   |                 | Application crash |
   |                 |                   |
   |                 | Attempt 2         |
   |                 |------------------>|
   |                 |                   |
   |                 |<------------------|
   |                 | Application crash |
   |                 |                   |
   |                 | Attempt 3         |
   |                 |------------------>|
   |                 |                   |
   |                 |<------------------|
   |                 |                   |
   |<----------------| Maximum attempts  |
   |                 | reached           |
```

The recovery manager currently limits automatic restart attempts to **three**.

---

## 6. Activity Diagram

```text
             +-------+
             | Start |
             +---+---+
                 |
                 v
       +-------------------+
       | Start Monitor     |
       +---------+---------+
                 |
                 v
       +-------------------+
       | Start Application |
       +---------+---------+
                 |
                 v
       +-------------------+
       | Collect Evidence  |
       +---------+---------+
                 |
                 v
       +-------------------+
       | Wait for Process   |
       +---------+---------+
                 |
                 v
            +----+----+
            | Process |
            | crashed?|
            +----+----+
              /       \
            No         Yes
            |           |
            v           v
     +-------------+  +----------------+
     | Report Exit |  | Detect Signal  |
     +------+------+  +-------+--------+
            |                 |
            |                 v
            |        +----------------+
            |        | Find Core Dump |
            |        +-------+--------+
            |                |
            |                v
            |        +----------------+
            |        | Run GDB        |
            |        | Analysis       |
            |        +-------+--------+
            |                |
            |                v
            |        +----------------+
            |        | Generate       |
            |        | Report         |
            |        +-------+--------+
            |                |
            |                v
            |        +----------------+
            |        | Restart        |
            |        | Application    |
            |        +-------+--------+
            |                |
            +-------+--------+
                    |
                    v
                  +---+
                  |End|
                  +---+
```

---

## 7. Component Diagram

```text
+------------------------------------------------------+
|              Crash Forensics System                  |
|                                                      |
|  +-------------+      +---------------------------+  |
|  | C++ Monitor |----->| Crash Forensics Engine    |  |
|  +-------------+      +-------------+-------------+  |
|                                      |                |
|                    +-----------------+---------------+
|                    |        |        |               |
|                    v        v        v               v
|             +----------+ +------+ +------+    +----------+
|             | Detector | |Proc  | | GDB  |    |Recovery  |
|             |          | |Data  | |      |    |Manager   |
|             +----------+ +------+ +------+    +----------+
|                    |        |        |               |
|                    +--------+--------+---------------+
|                             |
|                             v
|                      +-------------+
|                      | Crash Report|
|                      +-------------+
|                             |
|                             v
|                         reports/
|                                                      |
|  +-------------------+                               |
|  | CrashMon Interface|                               |
|  +---------+---------+                               |
|            |                                         |
+------------|-----------------------------------------+
             |
             v
       +-------------+
       | crashmon    |
       | Character   |
       | Driver      |
       +-------------+
             |
             v
        Linux Kernel
```

---

## 8. Deployment Diagram

```text
+-------------------------------------------------------+
|                   Development System                  |
|                                                       |
|  +------------------------------------------------+   |
|  |              Ubuntu / WSL2                     |   |
|  |                                                |   |
|  |  +----------------------+                      |   |
|  |  | Crash Monitor       |                      |   |
|  |  | C++ Application     |                      |   |
|  |  +----------+-----------+
```
8. Deployment Diagram
+-------------------------------------------------------+
|                   Development System                  |
|                                                       |
|  +------------------------------------------------+   |
|  |              Ubuntu / WSL2                     |   |
|  |                                                |   |
|  |  +----------------------+                      |   |
|  |  | Crash Monitor       |                      |   |
|  |  | C++ Application     |                      |   |
|  |  +----------+-----------+                      |   |
|  |             |                                  |   |
|  |       Linux System Interfaces                  |   |
|  |             |                                  |   |
|  |      +------+-------+                          |   |
|  |      |              |                          |   |
|  |     /proc         GDB                         |   |
|  |      |              |                          |   |
|  |      +------+-------+                          |   |
|  |             |                                  |   |
|  |       Core Dump Files                          |   |
|  |             |                                  |   |
|  |         reports/                               |   |
|  |                                                |   |
|  |  Character Driver Prototype                    |   |
|  +------------------------------------------------+   |
|                                                       |
+-------------------------------------------------------+
9. Data Flow

The major data flow through the system is:

Target Application
       |
       v
Process PID
       |
       v
/proc/<PID>
       |
       v
Process Evidence
       |
       +----------------------+
       |                      |
       v                      v
Crash Status              Core Dump
       |                      |
       |                      v
       |                  GDB Analysis
       |                      |
       +----------+-----------+
                  |
                  v
            Crash Report
                  |
                  v
          Recovery Manager
                  |
                  v
          Restart Application
10. UML Design Summary

The UML design represents the separation of responsibilities within the framework.

The CrashDetector identifies abnormal termination, EvidenceCollector gathers Linux process information, GDBAnalyzer performs post-mortem debugging, CrashReport persists forensic evidence, and RecoveryManager handles bounded automatic restart.

CrashMonInterface provides the abstraction for communication with the planned character-device driver.

The architecture follows a modular design so individual components can be tested independently and extended in future versions.
