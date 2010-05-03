//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Platform.hpp"
#include "Entry.hpp"

Platform*
rPlatform()
{
  static Platform* mPlatform = createPlatform();
  return mPlatform;
}

