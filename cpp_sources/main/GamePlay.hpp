//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef SHORTHIKE_GAMEPLAY_HPP
#define SHORTHIKE_GAMEPLAY_HPP

#include "util/State.hpp"

class Camera;
class Mission;

class GamePlay : public State
{
  friend GamePlay* rGamePlay();
public:
  GamePlay();

  void setMission(Mission* mission);
  Mission* getMission() {return mMission;}

  virtual void enterState();
  virtual void exitState();
  virtual void update(float timestep);
private:
  bool mActive;
  
  Camera* mCamera;
  Mission* mMission;
};

#endif
