//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef WIDGETS_CONTROL_DOCK_HPP
#define WIDGETS_CONTROL_DOCK_HPP

#include "guilib/Widget.hpp"
#include "guilib/ActionListener.hpp"

class Frame;
class GridLayout;
class Label;
class Button;
class ScriptButton;
class Station;


class ControlDock : public Singleton<ControlDock>, public Widget, public ActionListener
{
public:
  ControlDock();
  
  void setCurrentStation(Station* station) {currentStation = station;}

  virtual void update(Time deltaT);

  virtual void actionPerformed(Widget* source);
private:
  Station* currentStation;

  GridLayout* gridLayout;

  Frame* infoFrame;
  Label* infoLabel;

  Frame* menuFrame;

  Label* mTrialTimer;
  ScriptButton* mBuyButton;  
};



#endif
