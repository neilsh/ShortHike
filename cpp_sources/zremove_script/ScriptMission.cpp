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
#include "game/Station.hpp"
#include "property/PrototypeManager.hpp"
#include "main/Main.hpp"
#include "widgets/Debrief.hpp"
#include "widgets/ModuleDock.hpp"

//--------------------------------------------------------------------------------

int
mission_get_credits(LuaState* state)
{
  Mission* mission = rMain()->getMission();
  state->PushNumber(mission->getCredits());
  return 1;
}

int
mission_set_credits(LuaState* state)
{
  Mission* mission = rMain()->getMission();
  LuaObject lCredits = state->Stack(1);
  if(lCredits.IsNumber() == false) return 0;
  float credits = lCredits.GetFloat();
  mission->setCredits(credits);
  return 0;
}

int
mission_credit_transaction(LuaState* state)
{
  Mission* mission = rMain()->getMission();
  LuaObject lCredits = state->Stack(1);
  if(lCredits.IsNumber() == false) return 0;
  float credits = lCredits.GetFloat();
  mission->creditTransaction(credits);
  return 0;
}


int
mission_is_solvent(LuaState* state)
{
  Mission* mission = rMain()->getMission();
  state->PushBoolean(mission->isSolvent());
  return 1;
}


//--------------------------------------------------------------------------------

int
mission_enable_module(LuaState* state)
{
  string handle = stringFromLuaObject(state->Stack(1));
  if(handle != "") {
    rMain()->getMission()->enableModule(handle);
    ModuleDock::getSingleton().updatePrototypes();
  }
  return 0;
}

int
mission_disable_module(LuaState* state)
{
  string handle = stringFromLuaObject(state->Stack(1));
  if(handle != "") {
    rMain()->getMission()->disableModule(handle);
    ModuleDock::getSingleton().updatePrototypes();
  }
  return 0;
}

int
mission_is_module_enabled(LuaState* state)
{
  string handle = stringFromLuaObject(state->Stack(1));
  if(handle == "") return 0;
  state->PushBoolean(rMain()->getMission()->isModuleEnabled(handle));
  return 1;
}

int
mission_enable_all_modules(LuaState* state)
{
  map<string, Module*> prototypes = PrototypeManager::getSingleton().getPrototypes<Module>();
  for(map<string, Module*>::const_iterator iPrototype = prototypes.begin(); iPrototype != prototypes.end(); ++iPrototype) {
    string handle = (*iPrototype).first;
    rMain()->getMission()->enableModule(handle);
  }
  ModuleDock::getSingleton().updatePrototypes();
  return 0;
}

//--------------------------------------------------------------------------------

int
mission_set_time(LuaState* state)
{
  LuaObject lMonth = state->Stack(1);
  LuaObject lYear = state->Stack(2);
  if(!(lMonth.IsNumber() && lYear.IsNumber())) return 0;
  
  int month = lMonth.GetInteger();
  int year = lYear.GetInteger();
  rMain()->getMission()->setTime(month, year);
  return 0;
}


//--------------------------------------------------------------------------------


int
mission_complete(LuaState* state)
{
  Debrief::getSingleton().show();
  return 0;
}

//--------------------------------------------------------------------------------

void
registerMissionFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();

  globals.Register("mission_get_credits", &mission_get_credits);
  globals.Register("mission_set_credits", &mission_set_credits);
  globals.Register("mission_credit_transaction", &mission_credit_transaction);
  globals.Register("mission_is_solvent", &mission_is_solvent);
  
  globals.Register("mission_enable_module", &mission_enable_module);
  globals.Register("mission_disable_module", &mission_disable_module);
  globals.Register("mission_is_module_enabled", &mission_is_module_enabled);
  globals.Register("mission_enable_all_modules", &mission_enable_all_modules);

  globals.Register("mission_set_time", &mission_set_time);
  
  globals.Register("mission_complete", &mission_complete);
}
