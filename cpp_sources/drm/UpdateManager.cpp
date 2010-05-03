//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "UpdateManager.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

UpdateManager*
rUpdateManager()
{
  static UpdateManager* mUpdateManager = new UpdateManager();
  return mUpdateManager;
}
