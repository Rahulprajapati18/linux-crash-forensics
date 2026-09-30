cmake_minimum_required(VERSION 3.16)

project(LinuxCrashForensics
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(crash_monitor
    src/main.cpp
    src/crash_detector.cpp
    src/evidence_collector.cpp
    src/crash_report.cpp
    src/gdb_analyzer.cpp
    src/recovery_manager.cpp
    src/crashmon_interface.cpp
)

target_include_directories(crash_monitor
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(crash_monitor
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(crash_test
    tests/crash_test.cpp
)

target_compile_options(crash_test
    PRIVATE
    -g
)

add_executable(normal_test
    tests/normal_test.cpp
)

add_executable(evidence_test
    tests/evidence_test.cpp
    src/evidence_collector.cpp
)

target_include_directories(evidence_test
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(evidence_test
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
