//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
//
// Description:
// Parses input from the OGRE input mechanism and translates it into game specific
// events.
//
//--------------------------------------------------------------------------------



#ifndef USER_INTERFACE_STATE_HPP
#define USER_INTERFACE_STATE_HPP

class State
{
public:
  virtual void enterState() = 0;
  virtual void exitState() = 0;

  virtual void update(float timestep) {}
};


template<class StateClass>
class StateManager
{
public:

  StateManager()
    : currentState(NULL)
  {  
  }

  //------------------------------------------------------------
  // State access

  void
  addState(int identifier, StateClass* newState)
  {
    CHECK("Proposed identifier isn't used already", identifierToState[identifier] == NULL);
    identifierToState[identifier] = newState;
    stateToIdentifier[newState] = identifier;
  }


  StateClass*
  getState(int identifier)
  {
    return identifierToState[identifier];
  }


  //------------------------------------------------------------
  // Current state

  void
  setCurrentState(int identifier)
  {
    StateClass* newState = identifierToState[identifier];
    CHECK("State identifier is valid", newState != NULL);        
    setCurrentState(newState);
  }


  void
  setCurrentState(StateClass* newState)
  {
    if(newState != NULL)
      CHECK("Is the state managed by us", stateToIdentifier.find(newState) != stateToIdentifier.end());
    
    if(currentState != NULL)
      currentState->exitState();
    
    currentState = newState;
    
    if(currentState != NULL)
      currentState->enterState();
  }

  void
  clearState()
  {
    if(currentState != NULL)
      currentState->exitState();
    
    currentState = NULL;
  }


  StateClass*
  getCurrentState()
  {
    return currentState;
  }

  int getCurrentStateIdentifier()
  {
    return stateToIdentifier[currentState];
  }
  
  
private:
  StateClass* currentState;
  map<int, StateClass*> identifierToState;
  map<StateClass*, int> stateToIdentifier;
};


#endif
