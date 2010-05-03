//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef LAYERS_PROFILE_LAYER_HPP
#define LAYERS_PROFILE_LAYER_HPP

#include "guilib/Widget.hpp"

class Frame;
class Label;

class ProfileLayer : public Widget
{
public:
  ProfileLayer();
  virtual ~ProfileLayer();
  
  virtual void update(float);
private:
  Frame* mProfileViewer;
  Label* mProfileData;

  float mFPSTrend;
  float mTriTrend;
};


#endif
