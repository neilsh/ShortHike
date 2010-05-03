//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef LAYERS_STATION_LAYER_HPP
#define LAYERS_STATION_LAYER_HPP

#include "guilib/Widget.hpp"

class Mesh;
class Texture;
class Port;
class Module;
class Prototype;
class Station;
class Camera;

class StationLayer : public Widget
{
  friend StationLayer* rStationLayer();
public:
  void setStation(Station* station) {mStation = station;}
  void setCamera(Camera* camera) {mCamera = camera;}

  virtual void render(RenderSystem* renderSystem, int deltaX, int deltaY);
private:
  StationLayer();
  
  Station* mStation;
  Camera* mCamera;
};


#endif
