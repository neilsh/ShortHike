//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef DISCRETE_DIFFUSE_HPP
#define DISCRETE_DIFFUSE_HPP

#include "DiscreteComponent.hpp"

const float DIFFUSE_QUANTA = 2;
const float DIFFUSE_SPEED_FACTOR = 0.01;

class DiscreteDiffuse : public DiscreteComponent
{
public:
  DiscreteDiffuse(Module* module, int index = 0, float quanta = DIFFUSE_QUANTA, float speedfactor = DIFFUSE_SPEED_FACTOR);

  virtual float findTimeAdvance(float discreteTime);

  float getBaseLevel() const {return mBaseLevel;}
  void setBaseLevel(float baseLevel) {mBaseLevel = baseLevel;}
protected:
  float mBaseLevel;
};


#endif
