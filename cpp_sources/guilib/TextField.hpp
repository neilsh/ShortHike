//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//

#pragma once
#ifndef GUILIB_TEXT_FIELD_HPP
#define GUILIB_TEXT_FIELD_HPP

#include "Label.hpp"

class TextField : public Label
{
public:
  TextField(unsigned maxSize = 50);
  
  wstring getLabel();
  void setLabel(wstring newLabel);
  
  virtual bool keyboardEvent(char keyCode, bool pressed);
  virtual bool characterEvent(wchar_t character);
private:
  unsigned maxSize;
};

#endif
