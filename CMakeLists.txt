cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

enable_testing()

project(fsm)

add_subdirectory(demos)
add_subdirectory(libfsm)
add_subdirectory(external/googletest)
