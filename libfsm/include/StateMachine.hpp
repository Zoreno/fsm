/**
 * @file StateMachine.hpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-04-09
 *
 * @brief This header-only library defines a type-safe state machine for use
 * with user-type states and user-type events. Requires C++-17.
 *
 * @copyright Copyright (C) 2021, Joakim Bertils
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef _FSM_STATEMACHINE_HPP
#define _FSM_STATEMACHINE_HPP

#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <type_traits>

#ifdef LIBFSM_DEBUG
#include <iostream>
#endif

#define FSM_EVENT(EventName)              \
    class EventName : public ::fsm::Event \
    {                                     \
    };

#define FSM_TRANSITION(State, Event)                     \
    fsm::TransitionTo<State> Handle(const Event &) const \
    {                                                    \
        return {};                                       \
    }

#define FSM_STATE_BEGIN(StateName)        \
    class StateName : public ::fsm::State \
    {                                     \
    public:                               \
        std::string Name() override { return std::string(#StateName); }

#define FSM_STATE_END(StateName) \
    }

namespace fsm
{
    class State;
    class Event;

    /**
     * @brief Represents a transition to the same state as the previous
     * 
     */
    struct NullTransition
    {
        /**
         * @brief Function to be invoked on state change. Does nothing.
         * 
         * @tparam Machine Type of the state machine
         */
        template <typename Machine>
        void Execute(Machine *)
        {
#ifdef LIBFSM_DEBUG
            std::cout << "Called Null Transition" << std::endl;
#endif
        }
    };

    namespace detail
    {
        /**
         * @brief SFINAE function implementation for states where
         * state->Handle(event) is well-formed.
         *
         * @tparam State Type of the state.
         * @tparam Event Type of the event.
         * @param state The current state.
         * @param event The event to handle.
         * @param i Dummy parameter to make function call unambiguous.
         * @return TransitionTo object with specialization for the state to
         * transition to.
         */
        template <typename State, typename Event>
        auto InvokeHandleImpl(State *state, const Event &event, int i) -> decltype(state->Handle(event))
        {
            return state->Handle(event);
        }

        /**
         * @brief SFINAE function implementation for states where
         * state->Handle(event) is ill-formed.
         *
         * @tparam State Type of the state.
         * @tparam Event Type of the event.
         * @param state The current state.
         * @param event The event to handle.
         * @param d Dummy parameter to make function call unambiguous.
         * @return NullTransition object, causing the event to be ignored.
         */
        template <typename State, typename Event>
        auto InvokeHandleImpl(State *state, const Event &event, double d) -> NullTransition
        {
            return {};
        }

        /**
         * @brief Helper function to invoke the well-formed SFINAE
         * implementation from the above.
         *
         * This construct is to call Handle function specializations for the
         * event and call the NullTransition if not implemented. All states that
         * do not have a Handle function for the event will ignore the event.
         *
         * @tparam State Type of the state.
         * @tparam Event Type of the event.
         * @param state The current state.
         * @param event The current event.
         * @return A transition object, either TransitionTo or NullTransition.
         */
        template <typename State, typename Event>
        auto InvokeHandle(State *state, const Event &event)
        {
            // Call Impl with an int type, and if int function is not
            // well-formed, implicit type cast to double will occur.
            return InvokeHandleImpl(state, event, int{});
        }

        /**
         * @brief Checks that all types in @a Types fulfills the requirements
         * set by @a Trait with regard to the type @a Type.
         *
         * Trait<Type, T>::value where T is any type in @a Types has to be
         * well-formed and evaluate to a constexpr boolean expression. A type is
         * said to fulfill the requirement if Trait<Type, T>::value evaluates to
         * true.
         *
         * @tparam Type Type to validate against.
         * @tparam Trait Trait to fulfill, for example std::is_base_of.
         * @tparam Types List of parameters to check.
         */
        template <typename Type,
                  template <typename, typename> typename Trait,
                  typename... Types>
        struct ValidateType
        {
            /**
             * @brief True if all types in @a Types fulfills the @a Trait.
             *
             * The fold will expand to all trait values joined with the and
             * operator.
             */
            static constexpr bool value = (Trait<Type, Types>::value && ...);
        };

        /**
         * @brief Checks that all states in @a States is derived from
         * ::fsm::State.
         *
         * @tparam States State types to check.
         */
        template <typename... States>
        using IsDerivedFromState = ValidateType<::fsm::State,
                                                std::is_base_of,
                                                States...>;

        /**
         * @brief Checks that all Events in @a Events is derived from
         * ::fsm::Event.
         *
         * @tparam Events Event types to check.
         */
        template <typename... Events>
        using IsDerivedFromEvent = ValidateType<::fsm::Event,
                                                std::is_base_of,
                                                Events...>;

    } // namespace detail

    /**
     * @brief Base class for the state machine events.
     * 
     */
    class Event
    {
    public:
        /**
         * @brief Virtual destructor as the event is polymorphic.
         *     
         */
        virtual ~Event() = default;
    };

    /**
     * @brief Base class for a state in the state machine.
     * 
     */
    class State
    {
    public:
        /**
         * @brief Virtual destructor as the state is polymorphic.
         *     
         */
        virtual ~State() = default;

        /**
         * @brief Function to be called when entering the state.
         * 
         */
        virtual void OnStateEnter() {}

        /**
         * @brief Function to be called when exiting the state.
         *
         */
        virtual void OnStateExit() {}

        /**
         * @brief Gets an user-defined name of the state.
         * 
         * @return Sting containing the name of the state.
         */
        virtual std::string Name() { return "<Unnamed State>"; }
    };

    /**
     * @brief Class representing a FSM
     * 
     * @tparam States template parameter pack with the type of all the states in
     * the state machine.
     */
    template <typename... States>
    class StateMachine
    {
        // Check that all states in the state machine inherits from the state
        // base class.
        static_assert(detail::IsDerivedFromState<States...>::value,
                      "Some state in States does not inherit from ::fsm::State");

    public:
        /**
         * @brief Transitions the state machine to a new state.
         * 
         * @tparam State Type of the state to transition to.
         */
        template <typename State>
        void Transition()
        {
            static_assert(detail::IsDerivedFromState<State>::value);

            CallOnStateExit();
            currentState = &std::get<State>(states);
            CallOnStateEnter();
        }

        /**
         * @brief Handles an incoming event, passing it to the current state for
         * handling.
         * 
         * @tparam Event Type of the event.
         * @param event Event to pass to the current state.
         */
        template <typename Event>
        void Dispatch(const Event &event)
        {
            static_assert(detail::IsDerivedFromEvent<Event>::value,
                          "Event is not derived from fsm::Event");

            auto passEventToState = [this, &event](auto state_ptr) {
                detail::InvokeHandle(state_ptr, event).Execute(this);
            };

            std::visit(passEventToState, currentState);
        }

        /**
         * @brief Get the Name() of the current state.
         * 
         * @return String containing the current state's name.
         */
        std::string GetCurrentStateName()
        {
            auto getName = [](auto state_ptr) -> std::string {
                return state_ptr->Name();
            };

            return std::visit(getName, currentState);
        }

    private:
        /**
         * @brief Invokes the OnStateEnter function in the current state.
         * 
         */
        void CallOnStateEnter()
        {
            auto onStateEnter = [](auto state_ptr) {
                state_ptr->OnStateEnter();
            };

            return std::visit(onStateEnter, currentState);
        }

        /**
         * @brief Invokes the OnStateExit function in the current state.
         * 
         */
        void CallOnStateExit()
        {
            auto onStateExit = [](auto state_ptr) {
                state_ptr->OnStateExit();
            };

            return std::visit(onStateExit, currentState);
        }

        /**
         * @brief Tuple of all the states in the state machine.
         * 
         */
        std::tuple<States...> states{};

        /**
         * @brief Pointer to the current state in the state machine.
         *
         * A variant is used to preserve the type of the state, instead of
         * solving the problem with inheritance, where it is much harder to call
         * the correct Handle function without needing to implement vírtual
         * functions for all types in the base class.
         */
        std::variant<States *...> currentState{&std::get<0>(states)};
    };

    /**
     * @brief Helper type for defining transitions.
     *
     * Use as return types from the Handle function to define state transitions
     * for the state machine.
     *
     * @tparam State The state to transition to.
     */
    template <typename State>
    struct TransitionTo
    {
        /**
         * @brief Function to be invoked on state change.
         * 
         * @tparam Machine Type of the state machine
         */
        template <typename Machine>
        void Execute(Machine *machine)
        {
            static_assert(detail::IsDerivedFromState<State>::value);
            machine->template Transition<State>();
        }
    };

} // namespace fsm

#endif

//==============================================================================
// End of file
//==============================================================================