//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "ScriptButton.hpp"
#include "game/Mission.hpp"
#include "main/Main.hpp"
  
//--------------------------------------------------------------------------------

ScriptButton::ScriptButton(wstring label, string scriptCommand, float fontSize)
  : Button(label)
{
  mScriptCommand = scriptCommand;
  setFontSize(fontSize);
  addActionListener(this);
}


void
ScriptButton::actionPerformed(Widget*)
{
  rMain()->getMission()->doString(mScriptCommand);
}

