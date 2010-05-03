//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef LAYERS_INFO_BROWSER_HPP
#define LAYERS_INFO_BROWSER_HPP

#include "guilib/Widget.hpp"

class Frame;
class Button;
class Label;

class InfoBrowser : public Widget
{
public:
  InfoBrowser();
  virtual ~InfoBrowser();

  virtual void update(float timestep);
private:
  Frame* mInfoFrame;

  Button* mSelectionButton;
  Button* mStationButton;

  Label* mInfo;
};


#endif
