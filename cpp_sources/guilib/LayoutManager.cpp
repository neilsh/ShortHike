// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com

#include "Common.hpp"

#include "LayoutManager.hpp"

int
LayoutManager::preferredLayoutHeight(Widget* c, int)
{
  return preferredLayoutSize(c).w;
}


int
LayoutManager::preferredLayoutWidth(Widget* c, int)
{
  return preferredLayoutSize(c).h;
}
