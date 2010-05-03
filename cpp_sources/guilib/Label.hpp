//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#pragma once
#ifndef GUILIB_LABEL_HPP
#define GUILIB_LABEL_HPP


#include "Widget.hpp"
#include "Dimension.hpp"

class Graphics;

class Label : public Widget
{
public:
  Label();
  Label(int label, bool hasBorder = false);
  Label(wstring label, bool hasBorder = false);
  Label(wstring label, float fontSize, bool hasBorder = false);
  virtual ~Label();

  wstring getLabel();
  void setLabel(wstring newLabel);
  void setLabel(int num);
  void setBorder(bool hasBorder);
  void setFontSize(float fontSize);
  void setColor(float r, float g, float b);
  void setColor(ColorType color);
  enum VerticalAlignment {TOP, CENTER_VERTICAL, BOTTOM};
  enum HorizontalAlignment {LEFT, CENTER_HORIZONTAL, RIGHT};
  void setVerticalAlignment(VerticalAlignment newAlignment);
  void setHorizontalAlignment(HorizontalAlignment newAlignment);

  virtual void paint(Graphics* graphicsContext);
private:
  void init();
  void updatePreferredSize();
  
  bool hasBorder;
  VerticalAlignment verticalAlignment;
  HorizontalAlignment horizontalAlignment;
  float fontSize;
  ColorType textColor;
  wstring myLabel;
};


#endif
