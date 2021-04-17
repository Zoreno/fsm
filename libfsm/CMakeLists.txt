project(libfsm)

add_library(fsm INTERFACE)

target_include_directories(fsm INTERFACE include)

add_subdirectory(unittest)
