//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef GAME_GROUP_HPP
#define GAME_GROUP_HPP

class Module;

class Group
{
public:
  Group();

  void addModule(Module* newModule);

  void setModules(const set<Module*> modules);
  const set<Module*> getModules() const;
  void clear();

private:
  set<Module*> mModules;
};

#endif
