//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_DX7_TEXTURE_HPP
#define RENDERING_DX7_TEXTURE_HPP

#include "rendering/Texture.hpp"

class DX7RenderSystem;

class DX7Texture : public Texture
{
  friend class DX7RenderSystem;
public:
  virtual ~DX7Texture();

  virtual bool flush();

private:
  DX7Texture(DX7RenderSystem* renderSystem);
  void cleanup();
  
  DX7RenderSystem* mRenderSystem;

  IDirectDrawSurface7* mTextureSurface;
};


#endif
