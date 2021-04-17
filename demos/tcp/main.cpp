#include <iostream>

#include "StateMachine.hpp"

//==============================================================================
// Events
//==============================================================================

class SynEvent : fsm::Event
{
};

class SynAckEvent : fsm::Event
{
};

class AckEvent : fsm::Event
{
};

class FinEvent : fsm::Event
{
};

class FinAckEvent : fsm::Event
{
};

class RstEvent : fsm::Event
{
};

class TimeoutEvent : fsm::Event
{
};

class ActiveOpenEvent : fsm::Event
{
};

class PassiveOpenEvent : fsm::Event
{
};

class SendDataEvent : fsm::Event
{
};

class CloseEvent : fsm::Event
{
};

//==============================================================================
// States
//==============================================================================

class ClosedState;
class ListenState;
class SynRcvdState;
class SynSentState;
class EstablishedState;
class FinWait1State;
class FinWait2State;
class ClosingState;
class TimeWaitState;
class CloseWaitState;
class LastAckState;

class ClosedState : public fsm::State
{
public:
    fsm::TransitionTo<ListenState> Handle(const PassiveOpenEvent &) const
    {
        std::cout << "Got PassiveOpenEvent" << std::endl;
        return {};
    }

    fsm::TransitionTo<SynSentState> Handle(const ActiveOpenEvent &) const
    {
        return {};
    }
};

class ListenState : public fsm::State
{
public:
    fsm::TransitionTo<SynSentState> Handle(const SendDataEvent &) const
    {
        std::cout << "Sending data" << std::endl;
        return {};
    }

    fsm::TransitionTo<SynRcvdState> Handle(const SynEvent &) const
    {
        return {};
    }
};

class SynRcvdState : public fsm::State
{
public:
    fsm::TransitionTo<ClosedState> Handle(const TimeoutEvent &) const
    {
        return {};
    }

    fsm::TransitionTo<ListenState> Handle(const RstEvent &) const
    {
        return {};
    }

    fsm::TransitionTo<EstablishedState> Handle(const AckEvent &) const
    {
        return {};
    }

    fsm::TransitionTo<FinWait1State> Handle(const CloseEvent &) const
    {
        return {};
    }
};

class SynSentState : public fsm::State
{
public:
    fsm::TransitionTo<ClosedState> Handle(const CloseEvent &) const
    {
        return {};
    }

    fsm::TransitionTo<SynRcvdState> Handle(const SynEvent &) const
    {
        return {};
    }

    fsm::TransitionTo<EstablishedState> Handle(const SynAckEvent &) const
    {
        std::cout << "Got synack" << std::endl;
        return {};
    }
};

class EstablishedState : public fsm::State
{
public:
    void OnStateEnter() override
    {
        std::cout << "Entering EstablishedState" << std::endl;
    }

    FSM_TRANSITION(CloseWaitState, FinEvent);

    fsm::TransitionTo<FinWait1State> Handle(const CloseEvent &) const
    {
        return {};
    }
};

class FinWait1State : public fsm::State
{
public:
    fsm::TransitionTo<ClosingState> Handle(const FinEvent &) const
    {
        return {};
    }

    fsm::TransitionTo<FinWait2State> Handle(const AckEvent &) const
    {
        return {};
    }

    fsm::TransitionTo<TimeWaitState> Handle(const FinAckEvent &) const
    {
        return {};
    }
};

class FinWait2State : public fsm::State
{
public:
    fsm::TransitionTo<TimeWaitState> Handle(const FinEvent &) const
    {
        return {};
    }
};

class ClosingState : public fsm::State
{
public:
    fsm::TransitionTo<TimeWaitState> Handle(const AckEvent &) const
    {
        return {};
    }
};

class TimeWaitState : public fsm::State
{
public:
    fsm::TransitionTo<ClosedState> Handle(const TimeoutEvent &) const
    {
        return {};
    }
};

class CloseWaitState : public fsm::State
{
public:
    fsm::TransitionTo<LastAckState> Handle(const CloseEvent &) const
    {
        return {};
    }
};

class LastAckState : public fsm::State
{
public:
    fsm::TransitionTo<ClosedState> Handle(const AckEvent &) const
    {
        return {};
    }
};

//==============================================================================
// Main
//==============================================================================

using TcpStateMachine = fsm::StateMachine<ClosedState,
                                          ListenState,
                                          SynRcvdState,
                                          SynSentState,
                                          EstablishedState,
                                          FinWait1State,
                                          FinWait2State,
                                          ClosingState,
                                          TimeWaitState,
                                          CloseWaitState,
                                          LastAckState>;

int main()
{
    TcpStateMachine stateMachine;

    stateMachine.Dispatch(PassiveOpenEvent{});
    stateMachine.Dispatch(SendDataEvent{});
    stateMachine.Dispatch(SynAckEvent{});
}

//==============================================================================
// End of file
//==============================================================================
