/**
 * @file door.cpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-04-09
 *
 * @brief Implements a simple door with two states: open and closed. Open and
 * close event change the state.
 *
 * @copyright Copyright (C) 2021, This program is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version. This program is distributed
 * in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received
 * a copy of the GNU General Public License along with this program.  If not,
 * see <https: //www.gnu.org/licenses/>.
 *
 */

#include <iostream>

#include "StateMachine.hpp"

//==============================================================================
// Events
//==============================================================================

FSM_EVENT(OpenEvent);
FSM_EVENT(CloseEvent);

//==============================================================================
// States
//==============================================================================

struct ClosedState;
struct OpenState;

FSM_STATE_BEGIN(ClosedState);
FSM_TRANSITION(OpenState, OpenEvent);
FSM_STATE_END(ClosedState);

FSM_STATE_BEGIN(OpenState);
FSM_TRANSITION(ClosedState, CloseEvent);
FSM_STATE_END(OpenState);

//==============================================================================
// Main
//==============================================================================

using Door = fsm::StateMachine<ClosedState, OpenState>;

int main()
{
    Door door;

    door.Dispatch(OpenEvent{});
    door.Dispatch(CloseEvent{});

    std::cout << door.GetCurrentStateName() << std::endl;

    return 0;
}

//==============================================================================
// End of file
//==============================================================================
