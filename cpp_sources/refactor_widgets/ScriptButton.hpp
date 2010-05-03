//--------------------------------------------------------------------------------
//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#ifndef WIDGETS_SCRIPT_BUTTON_HPP
#define WIDGETS_SCRIPT_BUTTON_HPP

#include "guilib/Button.hpp"
#include "guilib/ActionListener.hpp"

class ScriptButton : public Button, public ActionListener
{
public:
  ScriptButton(wstring label, string scriptCommand, float fontSize = 12);  

  virtual void actionPerformed(Widget*);
private:
  string mScriptCommand;
};


#endif
