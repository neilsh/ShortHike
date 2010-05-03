//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef LAYERS_SKY_LAYER_HPP
#define LAYERS_SKY_LAYER_HPP

#include "guilib/Widget.hpp"

class Location;
class Camera;

class SkyLayer : public Widget
{
  friend SkyLayer* rSkyLayer();
public:
  void setLocation(Location* location) {mLocation = location;}
  Location* getLocation() {return mLocation;}

  void setCamera(Camera* camera) {mCamera = camera;}

  virtual void update(float timeStep);
  virtual void render(RenderSystem* renderSystem, int deltaX, int deltaY);
private:
  SkyLayer();

  Location* mLocation;
  Camera* mCamera;
};


#endif
