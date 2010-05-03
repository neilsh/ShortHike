//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#ifndef USER_INTERFACE_ORBIT_MAP_HPP
#define USER_INTERFACE_ORBIT_MAP_HPP

#include "guilib/RasterImage.hpp"

class Orbit;
class Graphics;

class OrbitMap : public RasterImage
{
public:
  OrbitMap(Orbit* orbitScreen, string imageName);

  void rePlot();

  virtual void paint(Graphics* gfxContext);
  virtual void update(Time deltaT);
private:
  Vector2 plotCurrentPosition();
  Vector2 plotPosition(Degree longitude, Degree latitude);

  Orbit* orbitScreen;

  float phase;
  float plotTimer;
  float drawTimer;
  
  list<Vector2> plot;
};



#endif
