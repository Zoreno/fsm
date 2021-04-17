project(tcp)

add_executable(tcp main.cpp)

target_link_libraries(tcp PRIVATE fsm)
