//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef WIDGETS_DEBUG_TOOL_HPP
#define WIDGETS_DEBUG_TOOL_HPP

#include "guilib/ActionListener.hpp"
#include "guilib/Frame.hpp"

class Label;
class Button;

class DebugTool : public Frame, public Singleton<DebugTool>
{
public:
  DebugTool();

  virtual void update(Time deltaT);
  
//   virtual void actionPerformed(Widget* source);
private:
  Label* mDebug;
};



#endif
