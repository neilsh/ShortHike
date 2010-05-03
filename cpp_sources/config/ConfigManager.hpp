//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef CONFIG_CONFIG_MANAGER_HPP
#define CONFIG_CONFIG_MANAGER_HPP

class Config;
class Player;
class Mission;

class ConfigManager : public Singleton<ConfigManager>
{
public:
  ConfigManager();

  void loadConfig(string fileName);
  void loadPlayer(string fileName);

  Config* getCurrentConfig();
  void setCurrentConfig(Config* newConfig);

  void saveConfig();
  void savePlayer();

  static Config* getCurrent();
  static Player* getPlayer();
  static Mission* getHomeBase();
private:
  virtual ~ConfigManager();

  Config* mConfig;
  Player* mPlayer;
};


#endif
