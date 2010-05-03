//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "ConfigManager.hpp"
#include "Config.hpp"
#include "Player.hpp"
#include "game/Mission.hpp"
#include "main/Main.hpp"
#include "property/Property.hpp"
#include "property/DataFile.hpp"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "Lmcons.h"
#endif


//--------------------------------------------------------------------------------
// Singleton implementation

template<> ConfigManager*
Singleton<ConfigManager>::ms_Singleton = 0;


//--------------------------------------------------------------------------------
// Initializing the main screen

ConfigManager::ConfigManager()
  : mConfig(NULL), mPlayer(NULL)
{
  logEngineInfo("ConfigManager created");
}

ConfigManager::~ConfigManager()
{
  logEngineInfo("ConfigManager deleted");
}


//--------------------------------------------------------------------------------
// Quick static accessors

Config*
ConfigManager::getCurrent()
{
  return getSingleton().getCurrentConfig();
}

Player*
ConfigManager::getPlayer()
{
  return getSingleton().mPlayer;
}

Mission*
ConfigManager::getHomeBase()
{
  return getSingleton().mPlayer->getHomeBase();
}



//--------------------------------------------------------------------------------

void
ConfigManager::loadConfig(string fileName)
{
  struct _stat fileInfo;
  DataFile* configFile;
  if(_stat(fileName.c_str(), &fileInfo) == 0 &&
     (configFile = new DataFile(stringToWString(fileName))) != NULL &&
     configFile->getRootItem() != NULL &&
     configFile->getRootItem()->getClassName() == Config::getStaticClassName()) {
    mConfig = configFile->getRootItem()->castToClass<Config>();
  }
  else {
    configFile = new DataFile();
    mConfig = new Config(configFile);
    configFile->setRootItem(mConfig);
    configFile->save(stringToWString(fileName));
  }
}


Config*
ConfigManager::getCurrentConfig()
{
  return mConfig;
}

void
ConfigManager::setCurrentConfig(Config* _currentConfig)
{
  mConfig = _currentConfig;
}

//--------------------------------------------------------------------------------
// Saves current configurations to disk

void
ConfigManager::saveConfig()
{
  mConfig->getDataFile()->save();
}


//--------------------------------------------------------------------------------


void
ConfigManager::loadPlayer(string fileName)
{
  struct _stat fileInfo;
  DataFile* playerFile;
  if(_stat(fileName.c_str(), &fileInfo) == 0 &&
     (playerFile = new DataFile(stringToWString(fileName))) != NULL &&
     playerFile->getRootItem() != NULL &&
     playerFile->getRootItem()->getClassName() == Player::getStaticClassName()) {
    mPlayer = playerFile->getRootItem()->castToClass<Player>();
  }
  else {
    wstring initialName = L"Apo Apsis";
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    TCHAR buffer[UNLEN + 1];
    DWORD bufSize = UNLEN + 1;
    if(GetUserName(buffer, &bufSize)) {
      initialName = stringToWString(string(buffer));
    }      
#endif
    playerFile = new DataFile();
    mPlayer = new Player(playerFile, initialName);
    Mission* homeBase = new Mission(playerFile);
    mPlayer->setHomeBase(homeBase);

    Mission* prevMission = rMain()->getMission();
    rMain()->setMission(homeBase);
    homeBase->initScriptFile("assets/missions/home_base.lua");
    rMain()->setMission(prevMission);
    
    playerFile->setRootItem(mPlayer);
    playerFile->save(stringToWString(fileName));
  }
}

void
ConfigManager::savePlayer()
{
  mPlayer->getDataFile()->save();
}
