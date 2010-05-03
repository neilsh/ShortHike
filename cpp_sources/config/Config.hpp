//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef CONFIG_CONFIG_HPP
#define CONFIG_CONFIG_HPP

#include "property/Item.hpp"

class License;

class Config : public Item
{
  PROPERTY_DECLARATION(Config);
public:
  Config(DataFile* dataFile);

  bool getShowLoadingItems() {return showLoadingItems;}
  void setShowLoadingItems(bool _show) {showLoadingItems = _show;}

  vector<Real> getLoadTicks() {return loadTicks;}
  void setLoadTicks(vector<Real> _loadTicks) {loadTicks = _loadTicks;}

  int getDisplayMonitor() {return displayMonitor;}
  int getAntiAliasing() {return antiAliasing;}

  // DEBUG: Shaders are just plain broken ..
  bool getDisableShaders() {return true;}
  void setDisableShaders(bool _shadersDisabled) {mDisableShaders = _shadersDisabled;}

  License* getLicense() const {return mLicense;}
  void setLicense(License* license) {mLicense = license;}

  bool isCompleteDistribution() {return mCompleteDistribution;}
  void setCompleteDistribution(bool complete) {mCompleteDistribution = complete;}

  bool isUpdateInProgress() {return mUpdateInProgress;}
  void setUpdateInProgress(bool updateInProgress) {mUpdateInProgress = updateInProgress;}
private:
  virtual ~Config() {}

  bool mUpdateInProgress;

  int displayMonitor;
  int antiAliasing;
  bool mDisableShaders;
  
  bool showLoadingItems;
  vector<Real> loadTicks;
  
  bool mCompleteDistribution;

  License* mLicense;
};


#endif
