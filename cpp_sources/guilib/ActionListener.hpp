//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//

#pragma once
#ifndef GUILIB_ACTION_LISTENER_HPP
#define GUILIB_ACTION_LISTENER_HPP

class Widget;

class ActionListener
{
public:
  virtual void actionPerformed(Widget* source) = 0;
};

#endif
