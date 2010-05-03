//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//

#pragma once
#ifndef GUILIB_BUTTON_LISTENER_HPP
#define GUILIB_BUTTON_LISTENER_HPP

class ButtonListener
{
public:
  virtual void buttonClicked(Button* targetButton, unsigned clickCount) = 0;
};

#endif
