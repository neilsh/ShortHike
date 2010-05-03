//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Player.hpp"
#include "drm/License.hpp"
#include "game/Mission.hpp"
#include "property/Property.hpp"

//--------------------------------------------------------------------------------

START_PROPERTY_DEFINITION(Player)
{
  addProperty(new WStringProperty(L"Display name of the player.", SetMember(mName)));
  addProperty(new StringProperty(L"Forum name of the player", SetMember(mForumName)));
  addProperty(new StringProperty(L"Player website URL", SetMember(mWebsite)));
  
  addProperty(new RealProperty(L"Total playing time", SetMember(mPlayTime)));
  addProperty(new RealProperty(L"Total experience gathered", SetMember(mExperience)));
  addProperty(new RealProperty(L"Largest mission bonus", SetMember(mLargestBonus)));
  addProperty(new WStringProperty(L"Largest mission bonus", SetMember(mLargestBonusMission)));

  addProperty(new IntProperty(L"Modules added", SetMember(mModulesAdded)));
  addProperty(new IntProperty(L"Modules moved", SetMember(mModulesMoved)));
  addProperty(new IntProperty(L"Modules removed", SetMember(mModulesDeleted)));
  addProperty(new RealProperty(L"Revolutions rotated ", SetMember(mRevolutionsRotated)));
  addProperty(new IntProperty(L"Assemblies created", SetMember(mAssembliesCreated)));

  addProperty(new ItemProperty(L"HomeBase mission", SetMember(mHomeBase), Mission::getStaticClassName()));  
  // FIXME: to store mission information
  //   addProperty(new StringSetProperty(L"Names of missions completed", SetMember(mMissionsCompleted)));
}
END_PROPERTY_DEFINITION

//--------------------------------------------------------------------------------

Player::Player(DataFile* dataFile, wstring name)
  : Item(dataFile), mName(name), mForumName(""), mWebsite(""),
    mPlayTime(0), mExperience(0), mLargestBonus(0), mLargestBonusMission(L""),
    mModulesAdded(0), mModulesMoved(0), mModulesDeleted(0), mRevolutionsRotated(0),
    mAssembliesCreated(0)
{
}

//--------------------------------------------------------------------------------

const wstring&
Player::getName() const
{
  return mName;
}

void
Player::setName(wstring newName)
{
  mName = newName;
}


Mission*
Player::getHomeBase()
{
  return mHomeBase;
}

void
Player::setHomeBase(Mission* newHomeBase)
{
  mHomeBase = newHomeBase;
}

