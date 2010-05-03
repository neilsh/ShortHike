//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef GUILIB_DIALOG_HPP
#define GUILIB_DIALOG_HPP

#include "Frame.hpp"
#include "GUIManager.hpp"

class Dialog : public Frame
{
public:
  Dialog()
    : Frame()
  {
    setVisible(false);
  }

  virtual void mouseClickOutsideBounds() {};
  virtual bool isFullScreen() {return false;}

  virtual void show()
  {
    if(getVisible()) return;
    setVisible(true);
//     rGUIManager()->pushDialog(this);
  }
  
  virtual void hide()
  {
//     rGUIManager()->popDialog();
    setVisible(false);
  }  
};

#endif
