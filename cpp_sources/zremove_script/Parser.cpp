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
#include "ScriptUtil.hpp"
#include "audio/SoundManager.hpp"
#include "audio/SoundEffect.hpp"
#include "audio/Ambient.hpp"
#include "audio/Track.hpp"
#include "widgets/MissionSelector.hpp"

//--------------------------------------------------------------------------------
// Parse sound effects

int
Parser::parseSoundEffect(LuaState* state)
{
  if(LuaManager::getParseConsumer() == NULL) return 0;
  try {
    logEngineInfo(string("LuaManager::parseSoundEffect - ") + LuaManager::getCurrentFileName());

    LuaStack stack(state);
    LuaObject specification = stack[1];
    
    CHECK("Sound name is string", specification["name"].IsString());
    string soundName = specification["name"].GetString();
    CHECK("Sound resource is string", specification["resource"].IsString());
    string resourceName = specification["resource"].GetString();

    SoundEffect* newEffect = new SoundEffect(resourceName);

    LuaManager::getParseConsumer()->consumeSoundEffect(soundName, newEffect);
  }
  catch(Exception& e) {
    logEngineError(string("Error parsing SoundEffect ") + LuaManager::getCurrentFileName() + ": " + e.getFullDescription());
  }
  return 0;
}

//--------------------------------------------------------------------------------
// Parse ambient sounds

int
Parser::parseAmbient(LuaState* state)
{
  if(LuaManager::getParseConsumer() == NULL) return 0;
  try {
    logEngineInfo(string("LuaManager::parseAmbient - ") + LuaManager::getCurrentFileName());

    LuaStack stack(state);
    LuaObject specification = stack[1];
    
    CHECK("Ambient name is string", specification["name"].IsString());
    string ambientName = specification["name"].GetString();
    CHECK("Ambient resource is string", specification["resource"].IsString());
    string resourceName = specification["resource"].GetString();
    
    Ambient* newAmbient = new Ambient(resourceName);
    if(specification["runningTime"].IsNumber())
      newAmbient->setRunningTime(specification["runningTime"].GetFloat());

    LuaManager::getParseConsumer()->consumeAmbient(ambientName, newAmbient);
  }
  catch(Exception& e) {
    logEngineError(string("Error parsing Ambient ") + LuaManager::getCurrentFileName() + ": " + e.getFullDescription());
  }
  return 0;
}


//--------------------------------------------------------------------------------
// Parse ambient sounds

int
Parser::parseTrack(LuaState* state)
{
  if(LuaManager::getParseConsumer() == NULL) return 0;

  try {
    logEngineInfo(string("LuaManager::parseTrack - ") + LuaManager::getCurrentFileName());
    
    LuaStack stack(state);
    LuaObject specification = stack[1];
    
    CHECK("Track resource is string", specification["resource"].IsString());
    string resourceName = specification["resource"].GetString();
    
    Track* newTrack = new Track(resourceName);
    LuaManager::getParseConsumer()->consumeTrack(newTrack);
  }
  catch(Exception& e) {
    logEngineError(string("Error parsing Track ") + LuaManager::getCurrentFileName() + ": " + e.getFullDescription());
  }
  return 0;
}

//--------------------------------------------------------------------------------

int
Parser::parseMission(LuaState* state)
{
  try {
    logEngineInfo(string("LuaManager::parseMission - ") + LuaManager::getCurrentFileName());
    
    LuaStack stack(state);
    LuaObject lSpec = stack[1];
    if(lSpec.IsTable() == false) {
      logEngineError("Table malformed");
      return 0;
    }
    string handle = stringFromLuaObject(lSpec["handle"]);
    wstring name = wstringFromLuaObject(lSpec["name"]);
    wstring desc = wstringFromLuaObject(lSpec["description"]);
    int order = 0;
    if(lSpec["order"].IsNumber())
      order = lSpec["order"].GetInteger();
    LuaObject lVisible = lSpec["visible"];
    if(lVisible.IsBoolean() && lVisible.GetBoolean() == false) {
      return 0;
    }

    
    // FIXME: This is totally bound to the assets directory and will fail to load anything else .. DUH!
    MissionSelector::getSingleton().addMission(handle, name, desc, string("assets/") + LuaManager::getCurrentFileName(), order);
  }
  catch(Exception& e) {
    logEngineError(string("Error parsing Mission ") + LuaManager::getCurrentFileName() + ": " + e.getFullDescription());
  }
  return 0;
}

int
Parser::parseTrigger(LuaState* state)
{
  return 0;
}

int
Parser::parseMessage(LuaState* state)
{
  return 0;
}


//--------------------------------------------------------------------------------

int
Parser::parseCampaign(LuaState* state)
{
  try {
    logEngineInfo(string("LuaManager::parseMission - ") + LuaManager::getCurrentFileName());
    
    LuaStack stack(state);
    LuaObject lSpec = stack[1];
    if(lSpec.IsTable() == false) {
      logEngineError("Table malformed");
      return 0;
    }

    MissionSelector* missionSelector = MissionSelector::getSingletonPtr();
    string handle = stringFromLuaObject(lSpec["handle"]);
    wstring name = wstringFromLuaObject(lSpec["name"]);
//     wstring desc = wstringFromLuaObject(lSpec["description"]);
    int order = 0;
    if(lSpec["order"].IsNumber())
      order = lSpec["order"].GetInteger();
    missionSelector->addCampaign(handle, name, order);
    
    for(int iGroup = 1; iGroup <= lSpec.GetCount(); ++iGroup) {
      LuaObject lGroupDef = lSpec[iGroup];
      if(!(lGroupDef.IsTable() || lGroupDef.IsString())) continue;

      vector<string> missionGroup;
      if(lGroupDef.IsString()) {
        missionGroup.push_back(stringFromLuaObject(lGroupDef));
        missionSelector->addCampaignGroup(handle, missionGroup);
      }
      else if(lGroupDef.IsTable()) {
        for(int iMission = 1; iMission <= lGroupDef.GetCount(); ++iMission) {
          LuaObject lMissionSpec = lGroupDef[iMission];
          if(lMissionSpec.IsString())
            missionGroup.push_back(stringFromLuaObject(lMissionSpec));
        }
        if(missionGroup.size() > 0)
          missionSelector->addCampaignGroup(handle, missionGroup);
      }
    }
  }
  catch(Exception& e) {
    logEngineError(string("Error parsing Mission ") + LuaManager::getCurrentFileName() + ": " + e.getFullDescription());
  }

  return 0;
}




//================================================================================
// Unit tests

TEST_FIXTURE_DEFINITION("unit/sript/TestParser", TestParser);

void
TestParser::setUp()
{
  LuaManager::getSingleton().setParseConsumer(this);
  module = NULL;
}


//--------------------------------------------------------------------------------
// Utility

void
TestParser::consumeModule(string , Module* newModule)
{
  module = newModule;
}


