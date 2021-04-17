#include "StateMachine.hpp"

#include "gtest/gtest.h"

//==============================================================================
// Test fixtures
//==============================================================================

class StateMachineTest : public ::testing::Test 
{
public:
    class State1;
    class State2;

    class Event1 : public fsm::Event {};
    class Event2 : public fsm::Event {};

    class State1 : public fsm::State
    {
    public:
        std::string Name() override { return "State1"; }

        fsm::TransitionTo<State2> Handle(const Event1& event)
        {
            return {};
        }
    };

    class State2 : public fsm::State
    {
    public:
        std::string Name() override { return "State2"; }

        fsm::TransitionTo<State1> Handle(const Event2& event)
        {
            return {};
        }
    };

    using StateMachine = fsm::StateMachine<State1, State2>;

};

//==============================================================================
// Tests
//==============================================================================

TEST_F(StateMachineTest, DispatchSingle)
{
    StateMachine machine;

    ASSERT_EQ(machine.GetCurrentStateName(), std::string("State1"));

    machine.Dispatch(Event1{});

    ASSERT_EQ(machine.GetCurrentStateName(), std::string("State2"));
}


TEST_F(StateMachineTest, DispatchEventIgnored)
{
    StateMachine machine;

    ASSERT_EQ(machine.GetCurrentStateName(), std::string("State1"));

    machine.Dispatch(Event2{});

    ASSERT_EQ(machine.GetCurrentStateName(), std::string("State1"));
}

//==============================================================================
// Main
//==============================================================================

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

//==============================================================================
// End of file
//==============================================================================
