# Git Workflow — Linux Application Crash Forensics & Automatic Recovery Framework

## 1. Version Control System

Git is used as the version control system for the project.

The repository contains the source code, test programs, driver prototype, header files, and project documentation.

Generated files such as executables, object files, core dumps, reports, and runtime state are excluded using `.gitignore`.

## 2. Repository Structure

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
    └── git_workflow.md
```

## 3. Basic Git Workflow

The development workflow is:

```text
Modify Code
    |
    v
Build
    |
    v
Run Tests
    |
    v
Review Changes
    |
    v
git add
    |
    v
git commit
    |
    v
git status
```

## 4. Repository Initialization

The project repository was initialized using:

```bash
git init
```

The initial source files were then staged and committed.

## 5. Initial Commit

The first project commit was created using:

```bash
git add .gitignore driver include src tests
git commit -m "Initial implementation of Linux crash forensics framework"
```

The initial commit contains the functional user-space crash-forensics framework and character-driver prototype.

## 6. Checking Repository Status

The repository state can be checked using:

```bash
git status
```

A clean repository is indicated by:

```text
nothing to commit, working tree clean
```

## 7. Viewing Commit History

The project history can be viewed using:

```bash
git log --oneline
```

This provides a compact list of project commits.

## 8. Staging Changes

When new files or modifications are ready:

```bash
git add <file>
```

or:

```bash
git add .
```

The staged changes can be reviewed using:

```bash
git status
```

## 9. Creating a Commit

After testing the changes:

```bash
git commit -m "Description of change"
```

Commit messages should clearly describe the implemented functionality.

Examples:

```text
Add crash evidence collection
Add GDB crash analyzer
Add automatic recovery manager
Add UML system design
Improve crash report generation
```

## 10. Ignored Files

The project uses `.gitignore` to prevent generated files from entering version control.

Ignored categories include:

* Compiled executables
* Object files
* Core dumps
* Kernel module build artifacts
* Runtime state files
* Generated crash reports
* Temporary editor files

This keeps the repository focused on source code and project documentation.

## 11. Development Branch

The current repository uses the `master` branch created during repository initialization.

For future collaborative development, feature branches can be used:

```bash
git checkout -b feature-name
```

After development and testing, the branch can be merged into the main development branch.

## 12. Recommended Commit Practice

Each commit should represent a logical project change.

Recommended sequence for future development:

```text
Commit 1
Initial implementation

Commit 2
System architecture and UML documentation

Commit 3
Testing improvements

Commit 4
Driver integration improvements

Commit 5
Final documentation and presentation preparation
```

## 13. Repository Verification

Before submitting the project, verify:

```bash
git status
```

```bash
git log --oneline
```

```bash
git ls-files
```

The repository should contain source code and documentation while generated runtime artifacts remain ignored.
