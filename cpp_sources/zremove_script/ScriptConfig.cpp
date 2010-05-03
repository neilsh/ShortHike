//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#include "Common.hpp"

#include "FunctionExporters.hpp"
#include "config/ConfigManager.hpp"
#include "config/Config.hpp"

//--------------------------------------------------------------------------------

int
config_distribution_standard(LuaState*)
{
  ConfigManager::getCurrent()->setCompleteDistribution(false);
  return 0;
}

//--------------------------------------------------------------------------------

int
config_distribution_complete(LuaState*)
{
  ConfigManager::getCurrent()->setCompleteDistribution(true);
  return 0;
}

//--------------------------------------------------------------------------------

void
registerConfigFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();

  globals.Register("config_distribution_standard", &config_distribution_standard);
  globals.Register("config_distribution_complete", &config_distribution_complete);
}
