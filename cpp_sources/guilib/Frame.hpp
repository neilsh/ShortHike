//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
// Frames are the minimal object rendered through OGRE. Each frame has an associated
// texture and uses CPU based software rendering for generating the data. The
// graphics hardware is then used for all the block alpha and other operations.
// Each frame has an associates Graphics context that is used by all other components
// to render themselves.
//--------------------------------------------------------------------------------

#pragma once
#ifndef GUILIB_FRAME_HPP
#define GUILIB_FRAME_HPP

#include "Widget.hpp"

class RenderSystem;
class Surface;

class Frame : public Widget
{
public:
  Frame();
  
  virtual void paintAll(Graphics* gfxContext);
  virtual void render(RenderSystem* renderSystem, int deltaX, int deltaY);

  virtual void setSize(const int w, const int h);
  virtual void setSize(Dimension d) {setSize(d.w, d.h);}

  virtual bool mouseDownEvent(int button, int x, int y) {return true;}
  virtual bool mouseUpEvent(int button, int x, int y) {return true;}
  virtual bool mouseClickEvent(int buttonID, unsigned clickCount, int x, int y) {return true;}

//   virtual void boundsUpdatedEvent();

  void centerOnScreen();
private:
  Surface* mSurface;
};


#endif

// First working version of old guilib based renderer. Back to the roots! :-D
