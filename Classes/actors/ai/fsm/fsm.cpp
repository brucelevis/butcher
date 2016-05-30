#include "fsm.h"
#include <actors/ai/fsm/states/fsm_state.h>

namespace butcher {

FSM::FSM(Ai* ai)
  : _ai(ai)
{
  for ( auto s : FSMStateType() )
  {
    auto state = FSMState::create(s);
    if ( state )
    {
      state->setAi(_ai);
      _states[s] = state;
    }
  }

  _currentState = _states[FSMStateType::IDLE];
}

void FSM::update()
{
  if ( _currentState )
    _currentState->update();
}

FSMStatePtr FSM::getCurrentState() const
{
  return _currentState;
}

void FSM::addState(FSMStatePtr state)
{
  _states[ state->getType() ] = state;
}

bool FSM::changeState(FSMStateType newState)
{
  auto it = _states.find( newState );
  if ( it != _states.end() && it->second && it->second->canEnter() )
  {
    _currentState->onExit();
    _currentState = it->second;
    _currentState->onEnter();
    return true;
  }
  return false;
}

}
