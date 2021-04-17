project(door)

add_executable(door main.cpp)

target_link_libraries(door PRIVATE fsm)