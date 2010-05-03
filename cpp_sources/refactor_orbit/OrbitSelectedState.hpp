//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#ifndef USER_INTERFACE_ORBIT_SELECTED_MODE_HPP
#define USER_INTERFACE_ORBIT_SELECTED_MODE_HPP

#include "OrbitState.hpp"

class OrbitSelectedState : public OrbitState
{
public:
  OrbitSelectedState(Orbit* _orbitScreen);

  virtual void enterState();
  virtual void exitState();
  
  virtual bool mouseClickEvent(int buttonID, unsigned clickCount, int x, int y);
  virtual bool keyboardEvent(char keyCode, bool pressed);
  
  void setSelection(Module* module);
  void addSelection(Module* module);
  void removeSelection(Module* module);
  void clearSelection();
  void emptySelection();

  virtual void outputString(wostream& out);
private:
  Module* getFirstModule();
  void updateHandles();
  
  set<Module*> selection;
};



#endif
