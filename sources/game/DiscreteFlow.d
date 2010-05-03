//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import DiscreteComponent;

const float FLOW_SENSITIVITY = 15;
const float FLOW_QUANTA = 5;
const float FLOW_SPEED_FACTOR = 2;


class DiscreteFlow : public DiscreteComponent
{

//--------------------------------------------------------------------------------

this(Module hostModule, int index = 0, float quanta = FLOW_QUANTA, float speedFactor = FLOW_SPEED_FACTOR, float sensitivity = FLOW_SENSITIVITY)
{
  super(hostModule, index, quanta, speedFactor);
  mSensitivity = sensitivity;
  mFlowTotal = 0;
  mLocalLevel = 0;
  mLocalLevelMax = 0;
}

//--------------------------------------------------------------------------------

float
getLocalLevel()
{
  return mLocalLevel * mSensitivity;
}

float
getLocalLevelMax()
{
  return mLocalLevelMax;
}
 
void
setLocalLevelMax(float localLevel)
{
  mLocalLevelMax = localLevel / mSensitivity;
}

void
setLocalLevelCurrent(float percentage)
{
  mLocalLevel = percentage * mLocalLevelMax;
  updateTimeAdvance(mLastUpdate);
}

float
getCurrentPerformance()
{
  float performance = 0;
  if(mLocalLevel < 0) {
    performance = mCurrentLevel / -mLocalLevel;
  }
  else {
    performance = 1 - mCurrentLevel / mLocalLevel;
  }
  performance = clamp(performance, 0, 1);
  return performance;
}


//--------------------------------------------------------------------------------

float
findTimeAdvance(float discreteTime)
{
  float totalInFlow = 0;
  float totalOutFlow = 0;
  
  mTargetLevel = 0;
  int connectedPorts = 0;

  Module[] ports = mModule.getConnections();
  for(int iPort = 0; iPort < ports.length; ++iPort) {
    Module other = ports[iPort];
    if(other !is null) {
      DiscreteComponent dComponent = other.getDiscreteComponent(mIndex);
      
      mTargetLevel += dComponent.mCurrentLevel;
      connectedPorts++;
      if(dComponent.mCurrentLevel > mCurrentLevel)
        totalInFlow += fabs(mSensitivity * (dComponent.mCurrentLevel - mCurrentLevel));
      else
        totalOutFlow += fabs(mSensitivity * (dComponent.mCurrentLevel - mCurrentLevel));
    }
  }

  // REFACTOR remove
//   const PortList& ports = mModule.getPorts();
//   for(PortIterator iPort = ports.begin(); iPort != ports.end(); ++iPort) {
//     Port* myPort = *iPort;
//     if(false == myPort.isConnected()) continue;
//     Module* otherModule = myPort.getConnection().getHost();
//     DiscreteComponent* dComponent = otherModule.getDiscreteComponent(mIndex);

//     mTargetLevel += dComponent.mCurrentLevel;
//     connectedPorts++;
//     if(dComponent.mCurrentLevel > mCurrentLevel)
//       totalInFlow += fabs(mSensitivity * (dComponent.mCurrentLevel - mCurrentLevel));
//     else
//       totalOutFlow += fabs(mSensitivity * (dComponent.mCurrentLevel - mCurrentLevel));
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
  mTargetLevel = clamp(mTargetLevel, 0, 1);
  mFlowTotal = max(totalInFlow, totalOutFlow);
    
  float difference = fabs(mTargetLevel - mCurrentLevel);
  float timeAdvance = INFINITE_TIME_ADVANCE;
  if(0 != difference) {
    float speed = mSpeedFactor * difference;
    timeAdvance = mQuanta / speed;
  }
  
  mModule.updateFlowTotal(discreteTime, cast(DiscreteResource)mIndex, mFlowTotal);

  return timeAdvance;
}

protected:
  float mSensitivity;
  float mFlowTotal;

  float mLocalLevel;
  float mLocalLevelMax;
};


