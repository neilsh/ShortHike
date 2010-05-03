//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef LAYERS_KEYBOARD_LAYER_HPP
#define LAYERS_KEYBOARD_LAYER_HPP

#include "guilib/Widget.hpp"

class KeyboardLayer : public Widget
{
  friend KeyboardLayer* rKeyboardLayer();
public:
  virtual bool keyboardEvent(char keyCode, bool pressed);
private:
  KeyboardLayer();
};


#endif
