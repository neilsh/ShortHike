//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#ifndef USER_INTERFACE_ORBIT_READY_MODE_HPP
#define USER_INTERFACE_ORBIT_READY_MODE_HPP

#include "OrbitState.hpp"

class OrbitReadyState : public OrbitState
{
public:
  OrbitReadyState(Orbit* _orbitScreen);

  virtual void enterState();
  virtual void exitState();  
  virtual void outputString(wostream& out);
};

#endif
