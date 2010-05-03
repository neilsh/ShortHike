//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#ifndef SCREENS_LOADING_SCREEN_HPP
#define SCREENS_LOADING_SCREEN_HPP

#include "widgets/Screen.hpp"

class LoadingDialog;
class Main;

class LoadingScreen : public Screen, public ResourceGroupListener, public Singleton<LoadingScreen>
{
public:
  LoadingScreen(Main* main);
  
  virtual void enterState();  
  virtual void exitState();
  
  virtual SceneManager* getSceneManager() {return sceneManager;}

  void startTask(wstring taskName, int itemNumber);
  void startTask(wstring taskName, vector<Real> tickPoints);
  static void tick(wstring itemName = L"");
  void endTask();
  
  vector<Real> getNormalizedTicks();
  
  virtual void resourceGroupScriptingStarted (const String &groupName, size_t scriptCount);
  virtual void scriptParseStarted (const String &scriptName);
  virtual void scriptParseEnded (void);
  virtual void resourceGroupScriptingEnded (const String &groupName);
  virtual void resourceGroupLoadStarted (const String &groupName, size_t resourceCount);
  virtual void resourceLoadStarted (const ResourcePtr &resource);
  virtual void resourceLoadEnded (void);
  virtual void worldGeometryStageStarted (const String &description);
  virtual void worldGeometryStageEnded (void);
  virtual void resourceGroupLoadEnded (const String &groupName);
private:
  void tickImpl(wstring itemName);
  
  LoadingDialog* loadingDialog;

  Main* main;
  Timer* loadingTimer;

  bool isLoading;
  string resourceGroupName;
  unsigned int currentTick;
  vector<Real> tickPoints;
  vector<Real> storedTickPoints;

  SceneManager* sceneManager;
  Camera* camera;
};


 
#endif
