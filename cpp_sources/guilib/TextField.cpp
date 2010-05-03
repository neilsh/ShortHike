//--------------------------------------------------------------------------------
//
// Space Station Manager
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "TextField.hpp"
#include "Graphics.hpp"
#include "FrameBackground.hpp"

TextField::TextField(unsigned maxSizeP)
{
  maxSize = maxSizeP;
  setLabel(L"");
  setBackground(new FrameBackground());
}

void
TextField::setLabel(wstring newLabel)
{
  Label::setLabel(newLabel + L"*");
}


wstring
TextField::getLabel()
{
  wstring currentLabel = Label::getLabel();
  wstring choppedLabel = currentLabel.substr(0, currentLabel.size() - 1);
  return choppedLabel;
}


//--------------------------------------------------------------------------------

bool
TextField::characterEvent(wchar_t character)
{
  wstring currentField = getLabel();
  if(character == VK_BACK) {
    if(currentField.size() > 0) {
      wstring choppedField = currentField.substr(0, currentField.size() - 1);
      setLabel(choppedField);
    }
    return true;
  }

  if(currentField.size() < maxSize) {
    currentField += character;
    setLabel(currentField);
    return true;
  }

  return false;
}



bool
TextField::keyboardEvent(char keyCode, bool pressed)
{
  // Implement controls through this interface and not Character
  return false;
}
 
