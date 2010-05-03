//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef DISCRETE_FLOW_HPP
#define DISCRETE_FLOW_HPP

#include "DiscreteComponent.hpp"

const float FLOW_SENSITIVITY = 15;
const float FLOW_QUANTA = 5;
const float FLOW_SPEED_FACTOR = 2;


class DiscreteFlow : public DiscreteComponent
{
public:
  DiscreteFlow(Module* module, int index = 0, float quanta = FLOW_QUANTA, float speedFactor = FLOW_SPEED_FACTOR, float sensitivity = FLOW_SENSITIVITY);

  virtual float findTimeAdvance(float discreteTime);

  float getLocalLevel() const;
  float getLocalLevelMax() const {return mLocalLevelMax;}
  void setLocalLevelMax(float localLevel);
  void setLocalLevelCurrent(float percentage);
  float getCurrentPerformance();
protected:
  float mSensitivity;
  float mFlowTotal;

  float mLocalLevel;
  float mLocalLevelMax;
};


#endif
