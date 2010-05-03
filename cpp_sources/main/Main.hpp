//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef SHORTHIKE_MAIN_HPP
#define SHORTHIKE_MAIN_HPP

//--------------------

class Mission;
class Aspects;
class Station;
class Camera;
class State;

class Main
{
  friend Main* rMain();
public:
  virtual ~Main();

  int mainLoop();
  void render();
  void renderAndUpdate();

  void quitApplication();
  void setActive(bool isActive);

  const float& getFrameTime() {return mFrameTime;}

  enum Mode {
    MODE_EMPTY,
    MODE_GAMEPLAY,
    MODE_MISSION_EDITOR,
    MODE_MODULE_EDITOR,
    MODE_LOCATION_EDITOR
  };  

  void setMode(Mode newMode);
  void setProfiling(bool profiling) {mProfiling = profiling;} 
  bool isProfiling() {return mProfiling;}
private:
  Main();

  bool recoverUpdateInProgress();
  void dispatchTime(float timeStep);

  bool isRunning;
  bool mIsActive;
  float mFrameTime;
  bool mProfiling;

  Mode mModeID;
  State* mMode;
};

#endif
