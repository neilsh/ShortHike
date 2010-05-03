//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef SHORTHIKE_LOCATION_EDITOR_HPP
#define SHORTHIKE_LOCATION_EDITOR_HPP

#include "util/State.hpp"

class Camera;
class Location;

class LocationEditor : public State
{
  friend LocationEditor* rLocationEditor();
public:
  LocationEditor();

  virtual void enterState();
  virtual void exitState();
private:  
  Camera* mCamera;
  Location* mLocation;
};

#endif
