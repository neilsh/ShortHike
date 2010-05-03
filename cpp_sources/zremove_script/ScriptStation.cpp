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

#include "game/Group.hpp"
#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "game/Station.hpp"
#include "main/Main.hpp"
#include "property/DataFile.hpp"
#include "widgets/FileDialog.hpp"

//--------------------------------------------------------------------------------

// int
// station_current(LuaState* state)
// {
//   state->PushLightUserData(rMain()->getMission()->getStation());
//   return 1;
// }

int
station_get_name(LuaState* state)
{
  Station* station = rMain()->getMission()->getStation();
  wstring stationName = station->getName();
  state->PushWString(stationName.c_str());
  return 1;
}

int
station_set_name(LuaState* state)
{
  wstring stationName = getWString(state, 1);
  if(stationName != L"") {
    rMain()->getMission()->getStation()->setName(stationName);
  }
  return 0;
}


//--------------------------------------------------------------------------------

int
station_add_module(LuaState* state)
{
  Station* station = rMain()->getMission()->getStation();
  if(station == NULL) return 0;
  Module* newModule = scriptGetItem<Module>(state, 1);
  if(newModule == NULL) return 0;
  station->addModule(newModule);
  return 0;
}

int
station_count_modules(LuaState* state)
{
  Station* station = rMain()->getMission()->getStation();
  if(station == NULL) return 0;
  state->PushInteger(station->getModuleCount());
  return 1;
}

//--------------------------------------------------------------------------------

int station_get_modules(LuaState* state)
{
  Station* station = rMain()->getMission()->getStation();
  if(station == NULL) return 0;
  string typeFilter = "";
  LuaObject lTypeFilter = state->Stack(1);
  if(lTypeFilter.IsString()) {
    typeFilter = lTypeFilter.ToString();
  }

  LuaObject moduleList;
  moduleList.AssignNewTable(state);
  int index = 1;
  const set<Module*>& modules = station->getModules();
  for(set<Module*>::const_iterator iModule = modules.begin(); iModule != modules.end(); ++iModule) {
    Module* module = *iModule;
    const string& moduleType = module->getPrototypeName();
    if(moduleType == typeFilter) {
      moduleList.SetLightUserData(index++, module);
    }
  }
  moduleList.PushStack();
  return 1;
}

//--------------------------------------------------------------------------------

int
station_save_as_group(LuaState* state)
{
  FileDialog::getSingleton().show(L"Select group name", L"Save", "station_save_group");
  return 0;
}


int
station_save_group(LuaState* state)
{
  wstring fileName = wstringFromLuaObject(state->Stack(1));
  if(fileName == L"") return 0;
  
  Station* station = rMain()->getMission()->getStation();
  DataFile* groupFile = new DataFile();
  Group* group = station->groupFromStation(groupFile);
  groupFile->setRootItem(group);
  groupFile->save(fileName);
  delete groupFile;
  return 0;
}

int
station_load_group(LuaState* state)
{
  wstring fileName = wstringFromLuaObject(state->Stack(1));
  if(fileName == L"") return 0;

  DataFile* groupFile = new DataFile(fileName);
  if(NULL == groupFile->getRootItem()) {
    logEngineWarn("station_load_group: File not found");
    return 0;
  }
  
  Group* group = groupFile->getRootItem()->castToClass<Group>();
  if(NULL == group) return 0;
  
  Station* station = rMain()->getMission()->getStation();
  station->insertGroup(group);
  delete groupFile;
  return 0;
}



//--------------------------------------------------------------------------------

void
registerStationFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();

  globals.Register("station_get_name", &station_get_name);
  globals.Register("station_set_name", &station_set_name);
  globals.Register("station_add_module", &station_add_module);
  globals.Register("station_count_modules", &station_count_modules);
  globals.Register("station_get_modules", &station_get_modules);

  globals.Register("station_save_as_group", &station_save_as_group);
  globals.Register("station_save_group", &station_save_group);
  globals.Register("station_load_group", &station_load_group);
}
