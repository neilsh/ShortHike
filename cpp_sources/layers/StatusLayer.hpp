//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef LAYERS_STATUS_LAYER_HPP
#define LAYERS_STATUS_LAYER_HPP

#include "guilib/Widget.hpp"

class Frame;
class Label;
class Mission;

class StatusLayer : public Widget
{
public:
  StatusLayer();
  virtual ~StatusLayer();
  
  virtual void update(float);
private:
  Frame* mStatusFrame;
  Label* mStatus;
  Mission* mMission;
};


#endif
