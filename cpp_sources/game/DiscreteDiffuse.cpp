//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "DiscreteDiffuse.hpp"
#include "Module.hpp"
#include "Port.hpp"
#include "Prototype.hpp"


//--------------------------------------------------------------------------------

DiscreteDiffuse::DiscreteDiffuse(Module* module, int index, float quanta, float speedFactor)
  : DiscreteComponent(module, index, quanta, speedFactor), mBaseLevel(0)
{
}


//--------------------------------------------------------------------------------

float
DiscreteDiffuse::findTimeAdvance(float)
{
  const float BASE_WEIGHT_START = 1;
  const float BASE_WEIGHT_RESISTANCE = 1;

  mTargetLevel = 0;
  float totalMass = 0;
  unsigned int baseWeight = BASE_WEIGHT_START;

  const vector<Module*>& ports = mModule->getConnections();
  for(unsigned int iPort = 0; iPort < ports.size(); ++iPort) {
    Module* other = ports[iPort];
    if(NULL != other) {
      mTargetLevel += other->getPrototype()->getSimFloat(SIM_MASS) * other->getDiscreteComponent(mIndex)->mCurrentLevel;
      totalMass += other->getPrototype()->getSimFloat(SIM_MASS);
      baseWeight++;
    }    
  }

  // REFACTOR Remove
//   const PortList& ports = mModule->getPorts();
//   for(PortIterator portI = ports.begin(); portI != ports.end(); ++portI) {
//     Port* currentPort = *portI;
//     if(currentPort->isConnected()) {
//       Module* otherModule = currentPort->getConnection()->getHost();
//       mTargetLevel += otherModule->getMass() * otherModule->getDiscreteComponent(mIndex)->mCurrentLevel;
//       totalMass += otherModule->getMass();
//       baseWeight++;
//     }
//   }

  baseWeight *= BASE_WEIGHT_RESISTANCE;
  mTargetLevel += mBaseLevel * mModule->getPrototype()->getSimFloat(SIM_MASS) * baseWeight;
  totalMass += mModule->getPrototype()->getSimFloat(SIM_MASS) * baseWeight;
  if(0 != totalMass)
    mTargetLevel /= totalMass;
  else
    mTargetLevel = mBaseLevel;

  // LS can never be more than base, Stress can never be less
  if(mIndex == DISCRETE_LIFESUPPORT) {
    if(mTargetLevel > mBaseLevel) {
      mTargetLevel = mBaseLevel;
    }
  }
  else {
    if(mTargetLevel < mBaseLevel) {
      mTargetLevel = mBaseLevel;
    }
  }
  
    
  float difference = fabs(mTargetLevel - mCurrentLevel);
  if(0 == difference) return INFINITE_TIME_ADVANCE;
  
  float speed = mSpeedFactor * difference;
  float timeAdvance = mQuanta / speed;

  return timeAdvance;
}

