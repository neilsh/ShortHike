//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#pragma once
#ifndef GUILIB_PROGRESS_BAR_HPP
#define GUILIB_PROGRESS_BAR_HPP


#include "Widget.hpp"
#include "Dimension.hpp"

class Graphics;

class ProgressBar : public Widget
{
public:
  ProgressBar();
  
  void setMax(float max);
  void setCurrent(float current);
  
  void setMax(int max) {setMax(static_cast<float>(max));}
  void setCurrent(int current) {setCurrent(static_cast<float>(current));}

  float getMax() const {return maxValue;}
  float getCurrent() const {return currentValue;}

  virtual void paint(Graphics* graphicsContext);
private:
  float maxValue;
  float currentValue;
};


#endif
