//--------------------------------------------------------------------------------
//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#pragma once
#ifndef GUILIB_BUTTON_HPP
#define GUILIB_BUTTON_HPP


#include "Label.hpp"

class ActionListener;

class Button : public Label
{
public:
  Button(wstring label = L"", ActionListener* action = NULL, float fontSize = 14);
  virtual ~Button();
  
  bool isEnabled() const;
  void setEnabled(bool isEnabled);

  void addActionListener(ActionListener* a);

  //   virtual bool mouseClickEvent(int button, unsigned clickCount, int x, int y);
  virtual bool mouseDownEvent(int button, int, int);
  virtual bool mouseUpEvent(int button, int, int);
  virtual void mouseEnterEvent();
  virtual void mouseLeaveEvent();

  virtual void paint(Graphics* graphicsContext);
  virtual void update(float timeStep);
private:
  void emitClick();

  vector<ActionListener*> actionListeners;
//   vector<ButtonListener*> buttonListeners;

  bool enabled;
  bool inside;

  bool mHeld;
  float mTimeSincePress;
  float mResidualTime;
};



class Test_Button : public TestFixture
{
public:
  TEST_FIXTURE_START(Test_Button);
  TEST_CASE(event_emission);
  TEST_FIXTURE_END();

  void event_emission();
};

#endif
