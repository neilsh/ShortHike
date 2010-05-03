//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#include "Common.hpp"

#include "LuaManager.hpp"
#include "Parser.hpp"
#include "FunctionExporters.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> LuaManager*
Singleton<LuaManager>::ms_Singleton = 0;

//--------------------------------------------------------------------------------


LuaManager::LuaManager()
  : consumer(NULL)
{
  luaState = LuaState::Create();

  LuaObject globals = luaState->GetGlobals();

#ifndef SELF_TEST
  globals.Register("Module", &Parser::parseModule);
  globals.Register("SoundEffect", &Parser::parseSoundEffect);
  globals.Register("Ambient", &Parser::parseAmbient);
  globals.Register("Track", &Parser::parseTrack);
  globals.Register("Mission", &Parser::parseMission);
  globals.Register("Trigger", &Parser::parseTrigger);
  globals.Register("Message", &Parser::parseMessage);
  globals.Register("Campaign", &Parser::parseCampaign);

  registerGameFunctions(luaState);
  registerDRMFunctions(luaState);
#endif

  scriptPatterns.push_back("*.lua");
  if(ResourceGroupManager::getSingletonPtr() != NULL)
    ResourceGroupManager::getSingletonPtr()->_registerScriptLoader(this);
}


LuaManager::~LuaManager()
{
  LuaState::Destroy(luaState);
}



//--------------------------------------------------------------------------------

void
LuaManager::doString(string cmd)
{
  if(currentFileName == "")
    currentFileName = "console";

  ostringstream augmentedCommand;
  augmentedCommand << "-- File: " << currentFileName << endl << cmd;
  
  // FIXME: Make better error handling and reporting system
  luaState->DoString(augmentedCommand.str().c_str());
  currentFileName = "";
}

//--------------------------------------------------------------------------------

void
LuaManager::doFile(string fileName)
{
  luaState->DoFile(fileName.c_str()); 
}


//--------------------------------------------------------------------------------

void
LuaManager::setParseConsumer(ParseConsumer* _consumer)
{
  consumer = _consumer;
}

ParseConsumer*
LuaManager::getParseConsumer()
{
  return getSingleton().consumer;
}


//--------------------------------------------------------------------------------
// Script parsing

const StringVector&
LuaManager::getScriptPatterns() const
{
  return scriptPatterns;
}


void
LuaManager::parseScript(DataStreamPtr& stream, const String&)
{
  currentFileName = stream->getName();
  doString(stream->getAsString());
}


Real
LuaManager::getLoadingOrder() const
{
  return 2000;
}

