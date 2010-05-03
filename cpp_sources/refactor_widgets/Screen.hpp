//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------



#ifndef USER_INTERFACE_SCREEN_HPP
#define USER_INTERFACE_SCREEN_HPP

#include "util/State.hpp"
#include "guilib/UserEventListener.hpp"

class Screen : public State, public UserEventListener
{
public:
  Screen();

  virtual void enterState();
  virtual void exitState();
  
  virtual void update(float ) {}
  virtual bool requireExclusiveInput() {return false;}

  virtual SceneManager* getSceneManager() = 0;
};


#endif
