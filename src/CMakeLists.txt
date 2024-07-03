cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_COMPILER C:\\MinGW\\bin\\g++.exe)
project(finnc)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_ARCH "-m64")
set(TARGET_64 ON)

# LLVM stuff
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(finnc main.cpp)

llvm_map_components_to_libnames(llvm_libs
    Analysis
    Core
    CodeGen
    ExecutionEngine
    InstCombine
    Object
    OrcJIT
    RuntimeDyld
    Support
    native
)

target_link_libraries(finnc ${llvm_libs})