//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#include "Common.hpp"

#include "FunctionExporters.hpp"
#include "drm/LimitDialog.hpp"
#include "game/Mission.hpp"
#include "main/Main.hpp"
#include "property/DataFile.hpp"
#include "widgets/MissionSelector.hpp"
#include "widgets/Screen.hpp"

//--------------------------------------------------------------------------------

int
game_mission_selector(LuaState* state)
{
  MissionSelector::getSingleton().show();
  return 0;
}

//--------------------------------------------------------------------------------

int
game_load(LuaState* state)
{
  LuaObject lFileName = state->Stack(1);
  if(false == lFileName.IsString()) return 0;
  string fileName = lFileName.GetString();
  MissionSelector::getSingleton().loadMission(fileName);
  return 0;
}

int
game_save(LuaState* state)
{
  LuaObject lFileName = state->Stack(1);
  if(false == lFileName.IsString()) return 0;
  string fileName = lFileName.GetString();
  rMain()->getMission()->getDataFile()->save(stringToWString(fileName));
  return 0;
}


//--------------------------------------------------------------------------------

int
game_quit(LuaState* state)
{
  if(showLimitDialog(true)) return 0;
  rMain()->quitApplication();
  return 0;
}


//--------------------------------------------------------------------------------

void
registerGameFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();

  globals.Register("game_mission_selector", &game_mission_selector);

  globals.Register("game_load", &game_load);
  globals.Register("game_save", &game_save);
  globals.Register("game_quit", &game_quit);
}
