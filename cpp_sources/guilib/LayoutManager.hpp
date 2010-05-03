//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//



#pragma once
#ifndef GUILIB_LAYOUT_MANAGER_HPP
#define GUILIB_LAYOUT_MANAGER_HPP

#include "Dimension.hpp"

class Widget;

class LayoutManager
{
public:
  virtual void doLayout(Widget *c) = 0;
  virtual Dimension preferredLayoutSize(Widget *c) = 0;
  
  virtual int preferredLayoutHeight(Widget* c, int targetWidth);
  virtual int preferredLayoutWidth(Widget* c, int targetHeight);
};


#endif
