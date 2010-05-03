//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Module;
private import Prototype;
private import DiscreteComponent;

const float DIFFUSE_QUANTA = 2;
const float DIFFUSE_SPEED_FACTOR = 0.01;

class DiscreteDiffuse : public DiscreteComponent
{

  float getBaseLevel()
  {
    return mBaseLevel;
  }
  
  void setBaseLevel(float baseLevel)
  {
    mBaseLevel = baseLevel;
  }

  //--------------------------------------------------------------------------------

  this(Module host, int index, float quanta = DIFFUSE_QUANTA, float speedFactor = DIFFUSE_SPEED_FACTOR)
  {
    super(host, index, quanta, speedFactor);
    mBaseLevel = 0;
  }


  //--------------------------------------------------------------------------------

  override float
  findTimeAdvance(float update)
  {
    const float BASE_WEIGHT_START = 1;
    const float BASE_WEIGHT_RESISTANCE = 1;

    mTargetLevel = 0;
    float totalMass = 0;
    float baseWeight = BASE_WEIGHT_START;

    Module[] ports = mModule.getConnections();
    for(uint iPort = 0; iPort < ports.length; ++iPort) {
      Module other = ports[iPort];
      if(other !is null) {
        mTargetLevel += other.getPrototype().getSimFloat(SimFloat.MASS) * other.getDiscreteComponent(mIndex).mCurrentLevel;
        totalMass += other.getPrototype().getSimFloat(SimFloat.MASS);
        baseWeight++;
      }    
    }

    // REFACTOR Remove
    //   const PortList& ports = mModule.getPorts();
    //   for(PortIterator portI = ports.begin(); portI != ports.end(); ++portI) {
    //     Port* currentPort = *portI;
    //     if(currentPort.isConnected()) {
    //       Module* otherModule = currentPort.getConnection().getHost();
    //       mTargetLevel += otherModule.getMass() * otherModule.getDiscreteComponent(mIndex).mCurrentLevel;
    //       totalMass += otherModule.getMass();
    //       baseWeight++;
    //     }
    //   }

    baseWeight *= BASE_WEIGHT_RESISTANCE;
    mTargetLevel += mBaseLevel * mModule.getPrototype().getSimFloat(SimFloat.MASS) * baseWeight;
    totalMass += mModule.getPrototype().getSimFloat(SimFloat.MASS) * baseWeight;
    if(0 != totalMass)
      mTargetLevel /= totalMass;
    else
      mTargetLevel = mBaseLevel;

    // LS can never be more than base, Stress can never be less
    if(mIndex == DiscreteResource.LIFESUPPORT) {
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


protected:
  float mBaseLevel;
}


