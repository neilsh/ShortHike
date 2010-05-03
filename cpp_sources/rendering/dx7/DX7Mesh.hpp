//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_DX7_MESH_HPP
#define RENDERING_DX7_MESH_HPP

#include "rendering/Mesh.hpp"

class DX7RenderSystem;

class DX7Mesh : public Mesh
{
  friend class DX7RenderSystem;
public:
  virtual ~DX7Mesh();

  virtual void flushInternal();

private:
  DX7Mesh(DX7RenderSystem* renderSystem, bool collisionSupport);
  void cleanup();
  
  DX7RenderSystem* mRenderSystem;

  IDirect3DVertexBuffer7* mVertexBuffer;
  WORD* mIndexBuffer;

  DWORD mVertexCount;
  DWORD mIndexCount;
};


#endif
