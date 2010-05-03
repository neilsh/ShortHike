//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef SCRIPT_FUNCTION_EXPORTERS_HPP
#define SCRIPT_FUNCTION_EXPORTERS_HPP

void registerUtilFunctions(LuaState* state);
void registerDRMFunctions(LuaState* state);
void registerGameFunctions(LuaState* state);
void registerConfigFunctions(LuaState* state);
void registerUIFunctions(LuaState* state);

void registerMissionFunctions(LuaState* state);
void registerAspectsFunctions(LuaState* state);
void registerStationFunctions(LuaState* state);
void registerModuleFunctions(LuaState* state);
void registerMessageFunctions(LuaState* state);


inline void registerAllFunctions(LuaState* state)
{
  registerUtilFunctions(state);
  registerDRMFunctions(state);
  registerGameFunctions(state);
  registerConfigFunctions(state);
  registerUIFunctions(state);

  registerMissionFunctions(state);
  registerAspectsFunctions(state);
  registerStationFunctions(state);  
  registerModuleFunctions(state);
  registerMessageFunctions(state);
}


#endif
