project(unittest)

add_executable(statemachine_test StateMachineTest.cpp)
add_test(NAME statemachine_test COMMAND statemachine_test)
target_link_libraries(statemachine_test PRIVATE fsm gtest)
