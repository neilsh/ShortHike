//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef CONFIG_PLAYER_HPP
#define CONFIG_PLAYER_HPP

#include "property/Item.hpp"

class Mission;

class Player : public Item
{
  PROPERTY_DECLARATION(Player);
public:
  Player(DataFile* dataFile, wstring name = L"");
  virtual ~Player() {}

  const wstring& getName() const;
  void setName(wstring newName);

  Mission* getHomeBase();
  void setHomeBase(Mission* newHomeBase);
  
  float mPlayTime;
  float mExperience;
  float mLargestBonus;
  wstring mLargestBonusMission;

  int mModulesAdded;
  int mModulesMoved;
  int mModulesDeleted;
  float mRevolutionsRotated;
  int mAssembliesCreated;
private:
  wstring mName;
  string mForumName;
  string mWebsite;
  
  set<string> mMissionsCompleted;

  Mission* mHomeBase;
};


#endif
