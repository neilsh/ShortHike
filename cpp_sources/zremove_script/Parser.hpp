//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------

#ifndef SCRIPT_PARSER_HPP
#define SCRIPT_PARSER_HPP

class SoundEffect;
class Track;
class Ambient;
class Module;
class Port;

#pragma warning(disable: 4100)

class ParseConsumer
{
public:
  virtual void consumeSoundEffect(string soundName, SoundEffect* ) {}
  virtual void consumeTrack(Track* newTrack) {}
  virtual void consumeAmbient(string ambientName, Ambient* newAmbient) {}
  virtual void consumeModule(string identifier, Module* newModule) {}
  virtual void consumePort(string identifier, Port* newPort) {}
};

class Parser
{
public:
  static int parseModule(LuaState* state);
  static int parseTableImpl(LuaState* state);
  static int parseSoundEffect(LuaState* state);
  static int parseAmbient(LuaState* state);
  static int parseTrack(LuaState* state);
  static int parseMission(LuaState* state);
  static int parseTrigger(LuaState* state);
  static int parseMessage(LuaState* state);
  static int parseCampaign(LuaState* state);
};


//--------------------------------------------------------------------------------

class TestParser : public TestFixture, public ParseConsumer
{
public:
  TEST_FIXTURE_START(TestParser);
  TEST_CASE(parse_module);
  TEST_FIXTURE_END();

  void setUp();

  void parse_module();

  virtual void consumeModule(string , Module* newModule);
private:
  Module* module;  
};



#endif
