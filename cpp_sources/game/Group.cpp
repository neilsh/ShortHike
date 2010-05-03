//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Group.hpp"
#include "Module.hpp"

//--------------------------------------------------------------------------------

Group::Group()
{
}


//--------------------------------------------------------------------------------

void
Group::addModule(Module* newModule)
{
  mModules.insert(newModule);
}

void
Group::setModules(const set<Module*> modules)
{
  mModules = modules;
}


const set<Module*>
Group::getModules() const
{
  return mModules;
}

void
Group::clear()
{
  mModules.clear();
}
