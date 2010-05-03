//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "RenderSystem.hpp"
#include "main/Platform.hpp"


RenderSystem*
rRenderSystem()
{
  static RenderSystem* mRenderSystem = rPlatform()->createRenderSystem();
  return mRenderSystem;
}
