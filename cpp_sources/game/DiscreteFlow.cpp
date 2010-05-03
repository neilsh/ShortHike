//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "DiscreteFlow.hpp"
#include "Module.hpp"
#include "Port.hpp"

//--------------------------------------------------------------------------------

DiscreteFlow::DiscreteFlow(Module* module, int index, float quanta, float speedFactor, float sensitivity)
  : DiscreteComponent(module, index, quanta, speedFactor),
    mSensitivity(sensitivity), mFlowTotal(0), mLocalLevel(0), mLocalLevelMax(0)
{
}

//--------------------------------------------------------------------------------

float
DiscreteFlow::getLocalLevel() const
{
  return mLocalLevel * mSensitivity;
}
 
void
DiscreteFlow::setLocalLevelMax(float localLevel)
{
  mLocalLevelMax = localLevel / mSensitivity;
}

void
DiscreteFlow::setLocalLevelCurrent(float percentage)
{
  mLocalLevel = percentage * mLocalLevelMax;
  updateTimeAdvance(mLastUpdate);
}

float
DiscreteFlow::getCurrentPerformance()
{
  float performance = 0;
  if(mLocalLevel < 0) {
    performance = mCurrentLevel / -mLocalLevel;
  }
  else {
    performance = 1 - mCurrentLevel / mLocalLevel;
  }
  performance = clamp<float>(performance, 0, 1);
  return performance;
}


//--------------------------------------------------------------------------------

float
DiscreteFlow::findTimeAdvance(float discreteTime)
{
  float totalInFlow = 0;
  float totalOutFlow = 0;
  
  mTargetLevel = 0;
  int connectedPorts = 0;

  const vector<Module*>& ports = mModule->getConnections();
  for(unsigned int iPort = 0; iPort < ports.size(); ++iPort) {
    Module* other = ports[iPort];
    if(NULL != other) {
      DiscreteComponent* dComponent = other->getDiscreteComponent(mIndex);
      
      mTargetLevel += dComponent->mCurrentLevel;
      connectedPorts++;
      if(dComponent->mCurrentLevel > mCurrentLevel)
        totalInFlow += fabs(mSensitivity * (dComponent->mCurrentLevel - mCurrentLevel));
      else
        totalOutFlow += fabs(mSensitivity * (dComponent->mCurrentLevel - mCurrentLevel));
    }
  }

  // REFACTOR remove
//   const PortList& ports = mModule->getPorts();
//   for(PortIterator iPort = ports.begin(); iPort != ports.end(); ++iPort) {
//     Port* myPort = *iPort;
//     if(false == myPort->isConnected()) continue;
//     Module* otherModule = myPort->getConnection()->getHost();
//     DiscreteComponent* dComponent = otherModule->getDiscreteComponent(mIndex);

//     mTargetLevel += dComponent->mCurrentLevel;
//     connectedPorts++;
//     if(dComponent->mCurrentLevel > mCurrentLevel)
//       totalInFlow += fabs(mSensitivity * (dComponent->mCurrentLevel - mCurrentLevel));
//     else
//       totalOutFlow += fabs(mSensitivity * (dComponent->mCurrentLevel - mCurrentLevel));
//   }


  if(0 == connectedPorts) {
    if(mLocalLevel < 0)
      mTargetLevel = 0;
    else
      mTargetLevel = 1;
  }
  else {
    mTargetLevel += mLocalLevel;
    mTargetLevel /= connectedPorts;
  }
  mTargetLevel = clamp<float>(mTargetLevel, 0, 1);
  mFlowTotal = max(totalInFlow, totalOutFlow);
    
  float difference = fabs(mTargetLevel - mCurrentLevel);
  float timeAdvance = INFINITE_TIME_ADVANCE;
  if(0 != difference) {
    float speed = mSpeedFactor * difference;
    timeAdvance = mQuanta / speed;
  }
  
  mModule->updateFlowTotal(discreteTime, static_cast<DiscreteResource>(mIndex), mFlowTotal);

  return timeAdvance;
}

