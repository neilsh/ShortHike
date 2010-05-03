//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#ifndef USER_INTERFACE_ORBIT_SCREEN_MODE_HPP
#define USER_INTERFACE_ORBIT_SCREEN_MODE_HPP

#include "guilib/UserEventListener.hpp"

class Orbit;
class Item;
class Module;
class Port;

class OrbitState : public State, public UserEventListener
{
public:
  OrbitState(Orbit* _orbitScreen);  

  virtual void update(float timeStep) {}

  virtual bool mouseClickEvent(int buttonID, unsigned clickCount, int x, int y);
  virtual bool mouseDownEvent(int button, int x, int y);
  virtual bool mouseUpEvent(int button, int x, int y);
  virtual void mouseMotionEvent(int x, int y, float deltaX, float deltaY, float deltaWheel);

  virtual bool keyboardEvent(char keyCode, bool pressed);

  virtual void outputString(wostream& stream) = 0;
protected:
  Item* pickItem(int x, int y);

  Orbit* orbitScreen;
};

#endif
