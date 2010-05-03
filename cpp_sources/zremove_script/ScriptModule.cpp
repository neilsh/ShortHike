//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#include "Common.hpp"

#include "FunctionExporters.hpp"
#include "ScriptUtil.hpp"
#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "main/Main.hpp"
#include "property/PrototypeManager.hpp"

//--------------------------------------------------------------------------------

int
module_create(LuaState* state)
{
  LuaObject lName = state->Stack(1);
  if(false == lName.IsString()) return 0;
  string name = lName.GetString();

  PrototypeManager& pManager = PrototypeManager::getSingleton();
  Module* newModule = pManager.createInstanceClass<Module>(rMain()->getMission()->getDataFile(), name);
  if(NULL == newModule) return 0;

  state->PushLightUserData(newModule);
  return 1;
}

//--------------------------------------------------------------------------------

int
module_is_online(LuaState* state)
{
  Module* module = scriptGetItem<Module>(state, 1);
  if(module == NULL) return 0;
  state->PushBoolean(module->isOnline());
  return 1;
}


//--------------------------------------------------------------------------------

void
registerModuleFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();

  globals.Register("module_create", &module_create);
  globals.Register("module_is_online", &module_is_online);
}
