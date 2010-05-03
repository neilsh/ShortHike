//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef WIDGETS_DEBRIEF_HPP
#define WIDGETS_DEBRIEF_HPP

#include "guilib/ActionListener.hpp"
#include "guilib/Dialog.hpp"

class Label;
class List;
class Button;

class Debrief : public Dialog, public ActionListener, public Singleton<Debrief>
{
public:
  Debrief();

  virtual void show();
  
  virtual void actionPerformed(Widget* source);
private:
  Label* mDebriefing;

  Button* mNextMission;
  Button* mReplay;
  Button* mHomeBase;
  Button* mMissionSelector;
};



#endif
