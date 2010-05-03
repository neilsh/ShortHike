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
#include "main/Main.hpp"
#include "widgets/MessageReader.hpp"


int
message_post(LuaState* state)
{
  wstring from = getWString(state, 1);
  wstring subject = getWString(state, 2);
  wstring body = getWString(state, 3);

  Mission* mission = rMain()->getMission();
  mission->postMessage(from, subject, body);
  return 0;
}


int
message_reader_show(LuaState* state)
{  
  MessageReader::getSingleton().show();
  return 0;
}

int
message_reader_select_new(LuaState* state)
{
  MessageReader::getSingleton().selectNewestMessage();
  return 0;
}



//--------------------------------------------------------------------------------

void
registerMessageFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();
  globals.Register("message_post", &message_post);
  globals.Register("message_reader_show", &message_reader_show);
  globals.Register("message_reader_select_new", &message_reader_select_new);  
}
