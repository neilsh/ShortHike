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

//--------------------------------------------------------------------------------

int
log(LuaState* state)
{
  LuaObject lLogMessage = state->Stack(1);
  if(false == lLogMessage.IsString()) return 0;
  string logMessage = lLogMessage.GetString();
  logGameInfo(string("LUA: ") + logMessage);
  return 0;
}

//--------------------------------------------------------------------------------

string stringFromLuaObject(LuaObject lResult)
{
  string result = "";
  if(lResult.IsString())
    result = lResult.GetString();
  else if(lResult.IsWString())
    result = wstringToString(lResult.GetWString());
  return result;
}

wstring wstringFromLuaObject(LuaObject lResult)
{
  wstring result = L"";
  if(lResult.IsWString())
    result = lResult.GetWString();
  else if(lResult.IsString())
    result = stringToWString(lResult.GetString());
  return result;
}


wstring
getWString(LuaState* state, int stackIndex)
{
  wstring result = L"";
  return wstringFromLuaObject(state->Stack(stackIndex));
}




//--------------------------------------------------------------------------------

void
registerUtilFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();
  globals.Register("log", &log);
}
