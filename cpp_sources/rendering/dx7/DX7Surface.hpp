//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_DX7_DX7SURFACE_HPP
#define RENDERING_DX7_DX7SURFACE_HPP

#include "rendering/Surface.hpp"

class DX7RenderSystem;

class DX7Surface : public Surface
{
  friend class DX7RenderSystem;
public:
  virtual ~DX7Surface();

  virtual void flush();
private:
  DX7Surface(DX7RenderSystem* renderSystem, int width, int height);
  void cleanup();  

  DX7RenderSystem* mRenderSystem;
  IDirectDrawSurface7* mSurface;  
};


#endif
