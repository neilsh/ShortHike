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

#ifndef SCRIPT_LUA_MANAGER_HPP
#define SCRIPT_LUA_MANAGER_HPP

class ParseConsumer;

class LuaManager : public ScriptLoader, public Singleton<LuaManager>
{
public:
  LuaManager();
  virtual ~LuaManager();

  void doString(string cmd);
  void doFile(string fileName);

  virtual const StringVector& getScriptPatterns() const;
  virtual void parseScript(DataStreamPtr& stream, const String& groupName);  
  virtual Real getLoadingOrder() const;

  void setParseConsumer(ParseConsumer* consumer);
  static ParseConsumer* getParseConsumer();

  static const string& getCurrentFileName() {return getSingleton().currentFileName;}
private:
  LuaState* luaState;
  string currentFileName;
  ParseConsumer* consumer;

  StringVector scriptPatterns;
};



#endif
