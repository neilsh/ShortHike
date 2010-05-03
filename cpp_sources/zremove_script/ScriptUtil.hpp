//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef SCRIPT_SCRIPT_UTIL_HPP
#define SCRIPT_SCRIPT_UTIL_HPP

template<class ItemClass>
ItemClass*
scriptGetItem(LuaState* state, int index)
{
  LuaObject lItem = state->Stack(index);
  if(false == lItem.IsLightUserData()) return NULL;
  return static_cast<Item*>(lItem.GetLightUserData())->castToClass<ItemClass>();
}


extern string stringFromLuaObject(LuaObject lResult);
extern wstring wstringFromLuaObject(LuaObject lResult);
extern wstring getWString(LuaState* state, int stackIndex);

#endif
