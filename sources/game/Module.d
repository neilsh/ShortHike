//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import DiscreteComponent;
private import DiscreteDiffuse;
private import DiscreteFlow;
private import Station;
private import Prototype;
private import Port;

//--------------------------------------------------------------------------------
// FIXME:
// - Sense when the whole station is flipping and adjust all values accordingly


wchar[] toString(DiscreteResource resource)
{
  if(resource == DiscreteResource.LIFESUPPORT)
    return "LifeSupport";
  else if(resource == DiscreteResource.STRESS)
    return "Stress";
  else if(resource == DiscreteResource.PASSENGERS)
    return "Passengers";
  else if(resource == DiscreteResource.VISITORS)
    return "Visitors";
  else if(resource == DiscreteResource.CREW)
    return "Crew";
  else if(resource == DiscreteResource.CARGO)
    return "Cargo";
  else if(resource == DiscreteResource.COMMODITIES)
    return "Commodities";
  else if(resource == DiscreteResource.SERVICE)
    return "Service";
  return "Unknown";
}


class Module
{
  void setStation(Station station)
  {
    mStation = station;
  }

  Station getStation()
  {
    return mStation;
  }

  Prototype getPrototype()
  {
    return mPrototype;
  }
  
  void setProfitResource(DiscreteResource resource, bool value)
  {
    mProfitResource[resource] = value;
  }
  
  float getProfit()
  {
    return mProfitCurrent;
  }


  // ----------------------------------------
  // Port handling

  bool isPortConnected(uint portIndex)
  {
    return cast(bool)(mConnections[portIndex] !is null);
  }
  
  int
  getConnectedPort(Module other)
  {
    for(int iConnection = 0; iConnection < mConnections.length; ++iConnection) {
      if(mConnections[iConnection] is other)
        return iConnection;
    }
    assert(CHECK_FAIL("Found no connected port"));
    return -1;
  }
  

  Module[] getConnections()
  {
    return mConnections;
  }

  Module getFirstConnection()
  {
    for(int iConnection = 0; iConnection < mConnections.length; ++iConnection) {
      if(mConnections[iConnection] !is null)
        return mConnections[iConnection];
    }
    return null;
  }

  int
  getFirstConnectedPort()
  {
    for(int iConnection = 0; iConnection < mConnections.length; ++iConnection) {
      if(mConnections[iConnection] !is null)
        return iConnection;
    }
    return -1;
  }



  //--------------------------------------------------------------------------------

  float flowFactor(DiscreteResource resource)
  {
    if(DiscreteResource.PASSENGERS == resource) return 0.15;
    if(DiscreteResource.VISITORS == resource) return 0.3;
    if(DiscreteResource.CREW == resource) return 0.2;
    if(DiscreteResource.SERVICE == resource) return 0.3;
    if(DiscreteResource.CARGO == resource) return 0.1;
    if(DiscreteResource.COMMODITIES == resource) return 0.2;
    return 0;
  }

  //--------------------------------------------------------------------------------

  this(Prototype prototype)
  {
    mPrototype = prototype;
    mStation = null;
    mPosition = Vector.ZERO;
    mOrientation = Quaternion.IDENTITY;
    mEnergyOnline = false;
    mThermalOnline = false;
    mProfitCurrent = 0;      

    mConnections.length = prototype.getPorts().length;

    mDiscreteComponents[DiscreteResource.LIFESUPPORT] = new DiscreteDiffuse(this, DiscreteResource.LIFESUPPORT);
    mDiscreteComponents[DiscreteResource.STRESS] = new DiscreteDiffuse(this, DiscreteResource.STRESS);
  
    mDiscreteComponents[DiscreteResource.PASSENGERS] = new DiscreteFlow(this, DiscreteResource.PASSENGERS);
    mDiscreteComponents[DiscreteResource.VISITORS] = new DiscreteFlow(this, DiscreteResource.VISITORS);
    mDiscreteComponents[DiscreteResource.CREW] = new DiscreteFlow(this, DiscreteResource.CREW);
    mDiscreteComponents[DiscreteResource.CARGO] = new DiscreteFlow(this, DiscreteResource.CARGO);
    mDiscreteComponents[DiscreteResource.COMMODITIES] = new DiscreteFlow(this, DiscreteResource.COMMODITIES);
    mDiscreteComponents[DiscreteResource.SERVICE] = new DiscreteFlow(this, DiscreteResource.SERVICE);
  
    for(int iRes = DiscreteResource.min; iRes <= DiscreteResource.max; ++iRes) {
      mFlowTotal[iRes] = 0;
      mProfitResource[iRes] = false;
    }  
  
    setLifeSupportBase(mPrototype.getSimFloat(SimFloat.LIFESUPPORT_BASE));
    setFlowLocal(DiscreteResource.PASSENGERS, mPrototype.getSimFloat(SimFloat.FLOW_PASSENGERS));
    setFlowLocal(DiscreteResource.VISITORS, mPrototype.getSimFloat(SimFloat.FLOW_VISITORS));
    setFlowLocal(DiscreteResource.CREW, mPrototype.getSimFloat(SimFloat.FLOW_CREW));
    setFlowLocal(DiscreteResource.SERVICE, mPrototype.getSimFloat(SimFloat.FLOW_SERVICE));
    setFlowLocal(DiscreteResource.CARGO, mPrototype.getSimFloat(SimFloat.FLOW_CARGO));
    setFlowLocal(DiscreteResource.COMMODITIES, mPrototype.getSimFloat(SimFloat.FLOW_COMMODITIES));

    setProfitResource(DiscreteResource.PASSENGERS, mPrototype.getSimBool(SimBool.PROFIT_PASSENGERS));
    setProfitResource(DiscreteResource.VISITORS, mPrototype.getSimBool(SimBool.PROFIT_VISITORS));
    setProfitResource(DiscreteResource.CREW, mPrototype.getSimBool(SimBool.PROFIT_CREW));
    setProfitResource(DiscreteResource.SERVICE, mPrototype.getSimBool(SimBool.PROFIT_SERVICE));
    setProfitResource(DiscreteResource.CARGO, mPrototype.getSimBool(SimBool.PROFIT_CARGO));
    setProfitResource(DiscreteResource.COMMODITIES, mPrototype.getSimBool(SimBool.PROFIT_COMMODITIES));

    // DEBUG
    setFlowLocal(DiscreteResource.PASSENGERS, fnrand() * 10 + 6);
    setFlowLocal(DiscreteResource.VISITORS, fnrand() * 10 + 6);
    setFlowLocal(DiscreteResource.CREW, fnrand() * 10 + 6);
    setFlowLocal(DiscreteResource.SERVICE, fnrand() * 10 + 6);
    setFlowLocal(DiscreteResource.CARGO, fnrand() * 10 + 6);
    setFlowLocal(DiscreteResource.COMMODITIES, fnrand() * 10 + 6);
  }


  //--------------------------------------------------------------------------------


  void
  setPosition(Vector _position)
  {
    mPosition = _position;
  }


  Vector
  getPosition()
  {
    return mPosition;
  }

  Vector
  getWorldPosition()
  {
    return mPosition;
  }



  void
  setOrientation(Quaternion _orientation)
  {
    mOrientation = _orientation;
    mOrientation.normalize();
  }


  Quaternion
  getOrientation()
  {
    return mOrientation;
  }


  //--------------------------------------------------------------------------------

  Matrix
  getTransform()
  {
    return createMatrix(getPosition(), getOrientation());
  }


  void
  setTransform(Matrix transform)
  {
    setOrientation(transform.getOrientation());
    setPosition(transform.getPosition());
  }


  //--------------------------------------------------------------------------------

  bool
  canRotate()
  {
    int connections = 0;
    for(uint iPort = 0; iPort < mConnections.length; ++iPort) {
      if(null !is mConnections[iPort])
        connections++;
    }
    return cast(bool)(connections == 1);
  }


  // --------------------------------------------------------------------------------

  void
  rotate(float angle)
  {
    assert(CHECK("Rotation valid", canRotate));
    int pivotPort = -1;
    for(uint iPort = 0; iPort < mConnections.length; ++iPort) {
      if(null !is mConnections[iPort]) {
        pivotPort = iPort;
        break;
      }
    }

//     Matrix portToModule = createMatrix(connectionPort.getPosition(), connectionPort.getOrientation());
//     Matrix rotateModule = createMatrix(Vector.ZERO, createQuaternion(Vector.UNIT_Z, radFromDeg(angle)));
//     Matrix moduleToPort = portToModule.inverse();
//     Matrix currentRotation = createMatrix(mOrientation);
    Matrix newRotation = mPrototype.getRotateAroundPort(pivotPort, angle) * createMatrix(mOrientation);
    setOrientation(newRotation.getOrientation());
    mStation.connectModule(this);
//     Matrix newRotation = moduleToPort * rotateModule * portToModule * createMatrix(mOrientation);
    
  }
  
  // --------------------------------------------------------------------------------
  // Flips module to use next port
  
  void
  flipPort()
  {
    assert(CHECK("Rotation possible", canRotate));
    int pivotPort = getFirstConnectedPort();
    Module otherModule = mConnections[pivotPort];
    int otherPort = otherModule.getConnectedPort(this);

    pivotPort =  (pivotPort + 1) % mConnections.length;
    Matrix portTransform = mPrototype.getTransformAgainstPort(pivotPort, otherModule.mPrototype, otherPort, 0); 

    mStation.disconnectModule(this);
    setTransform(portTransform * otherModule.getTransform());
    mStation.connectModule(this);
  }
  

  //--------------------------------------------------------------------------------
  // Ports

  void
  connectPort(uint iMyPort, Module otherModule, uint iOtherPort)
  {
    disconnectPort(iMyPort);
    mConnections[iMyPort] = otherModule;
    otherModule.disconnectPort(iOtherPort);
    otherModule.mConnections[iOtherPort] = this;
  }


  void
  disconnectPort(uint iPort)
  {
    Module other = mConnections[iPort];
    if(null !is other) {
      for(int iOtherPort = 0; iOtherPort < other.mConnections.length; ++iOtherPort) {
        if(other.mConnections[iOtherPort] is this)
          other.mConnections[iOtherPort] = null;
      }      
    }
    mConnections[iPort] = null;
  }


  void
  disconnectAllPorts()
  {
    for(uint iPort = 0; iPort < mConnections.length; ++iPort) {
      disconnectPort(iPort);
    }
  }


  Matrix
  getFullPortTransform(uint portIndex)
  {
    return mPrototype.getPortTransform(portIndex) * getTransform();
  }


  //--------------------------------------------------------------------------------

  bool
  isOnline()
  {
    return cast(bool)(isEnergyOnline() && isThermalOnline());
  }

  void
  setOnline(bool online)
  {
    mEnergyOnline = online;
    mThermalOnline = online;
  }

  bool
  isEnergyOnline()
  {
    if(mPrototype.getSimFloat(SimFloat.ENERGY) >= 0) return true;
    return mEnergyOnline;
  }

  void
  setEnergyOnline(bool online)
  {
    mEnergyOnline = online;
  }

  bool
  isThermalOnline()
  {
    if(mPrototype.getSimFloat(SimFloat.THERMAL) >= 0) return true;
    return mThermalOnline;
  }

  void
  setThermalOnline(bool online)
  {
    mThermalOnline = online;
  }


  // --------------------------------------------------------------------------------

  float
  getLifeSupport()
  {
    return mDiscreteComponents[DiscreteResource.LIFESUPPORT].getCurrentLevel();
  }

  float
  getLifeSupportBase()
  {
    return (cast(DiscreteDiffuse)mDiscreteComponents[DiscreteResource.LIFESUPPORT]).getBaseLevel();
  }


  void
  setLifeSupportBase(float baseLifeSupport)
  {
    DiscreteDiffuse lsDiffuse = cast(DiscreteDiffuse)mDiscreteComponents[DiscreteResource.LIFESUPPORT];
    lsDiffuse.setBaseLevel(baseLifeSupport);
    lsDiffuse.setTargetLevel(baseLifeSupport);
  }

  // --------------------------------------------------------------------------------


  float
  getStress()
  {
    return mDiscreteComponents[DiscreteResource.STRESS].getCurrentLevel();
  }

  float
  getStressBase()
  {
    return (cast(DiscreteDiffuse)mDiscreteComponents[DiscreteResource.STRESS]).getBaseLevel();
  }


  void
  setStressBase(float baseStress)
  {
    (cast(DiscreteDiffuse)mDiscreteComponents[DiscreteResource.LIFESUPPORT]).setBaseLevel(baseStress);
  }

  // --------------------------------------------------------------------------------

  float
  getDiscreteCurrent(DiscreteResource resource)
  {
    return getDiscreteComponent(resource).getCurrentLevel();
  }

  float
  getDiscreteTarget(DiscreteResource resource)
  {
    return getDiscreteComponent(resource).getTargetLevel();
  }

  // --------------------------------------------------------------------------------

  float
  getFlowLocal(DiscreteResource resource)
  {
    return (cast(DiscreteFlow)getDiscreteComponent(resource)).getLocalLevel() / flowFactor(resource);
  }

  void
  setFlowLocal(DiscreteResource resource, float value)
  {
    DiscreteFlow flowComponent = cast(DiscreteFlow)getDiscreteComponent(resource);
    flowComponent.setLocalLevelMax(value * flowFactor(resource));

    if(value < 0)
      flowComponent.setLocalLevelCurrent(1);
    else
      flowComponent.setLocalLevelCurrent(0);
  }


  float
  getFlowTotal()
  {
    float totalFlow = 0;
    for(int iRes = DiscreteResource.min; iRes <= DiscreteResource.max; ++iRes)
      totalFlow += mFlowTotal[iRes];
    return totalFlow;
  }

  void
  updateFlowTotal(float discreteTime, DiscreteResource resource, float flow)
  {
    mFlowTotal[resource] = flow;

    float utilization = 0;
    if(mPrototype.getSimFloat(SimFloat.FLOW_CAPACITY) > 0)
      utilization = getFlowTotal() / mPrototype.getSimFloat(SimFloat.FLOW_CAPACITY);
    utilization = clamp(utilization, 0, 1);
    float stress = utilization * utilization * 5;
    if(stress > 10) stress = 10;

    DiscreteDiffuse stressComponent = cast(DiscreteDiffuse)getDiscreteComponent(DiscreteResource.STRESS);
    stressComponent.setBaseLevel(stress);
    stressComponent.updateTimeAdvance(discreteTime);
  }



  //--------------------------------------------------------------------------------

  void
  advanceTurn()
  {
    // Find minimum performance input
    float minimumPerformance = 1;
    for(int iRes = DiscreteResource.FLOW_INITIAL; iRes <= DiscreteResource.max; ++iRes) {
      DiscreteFlow flowComponent = cast(DiscreteFlow)getDiscreteComponent(iRes);
      if(flowComponent.getLocalLevelMax() < 0) {
        flowComponent.setLocalLevelCurrent(1);
        float currentPerformance = flowComponent.getCurrentPerformance();
        if(currentPerformance < minimumPerformance)
          minimumPerformance = currentPerformance;
      }
    }

    // Set all output to min performance
    for(int iRes = DiscreteResource.FLOW_INITIAL; iRes <= DiscreteResource.max; ++iRes) {
      DiscreteFlow flowComponent = cast(DiscreteFlow)getDiscreteComponent(iRes);
      if(flowComponent.getLocalLevelMax() >= 0) {
        flowComponent.setLocalLevelCurrent(minimumPerformance);
      }
    }
  
    // Find profit
    float minProfitPerformance = 1;
    if(mProfitResource[DiscreteResource.LIFESUPPORT]) {
      float lsPerformance = getLifeSupport() / getLifeSupportBase() * 0.1;
      if(lsPerformance < minProfitPerformance)
        minProfitPerformance = lsPerformance;
    }
    for(int iRes = DiscreteResource.FLOW_INITIAL; iRes <= DiscreteResource.max; ++iRes) {
      if(mProfitResource[iRes]) {
        DiscreteFlow flowComponent = cast(DiscreteFlow)getDiscreteComponent(iRes);
        float flowPerformance = flowComponent.getCurrentPerformance();
        if(flowPerformance < minProfitPerformance)
          minProfitPerformance = flowPerformance;
      }
    }
    mProfitCurrent = minProfitPerformance * mPrototype.getSimFloat(SimFloat.PROFIT_MAX);
  }


  //--------------------------------------------------------------------------------

  DiscreteComponent
  getDiscreteComponent(int index)
  {
    assert(CHECK("Valid DES index", index >= DiscreteResource.min && index <= DiscreteResource.max));
    return mDiscreteComponents[index];
  }

  void
  preloadDES()
  {
    for(int iRes = DiscreteResource.FLOW_INITIAL; iRes <= DiscreteResource.max; ++iRes) {
      DiscreteFlow flowComponent = cast(DiscreteFlow)getDiscreteComponent(iRes);
      float averageLevel = 0;
      int numConnections = 0;

      for(uint iPort = 0; iPort < mConnections.length; ++iPort) {
        Module other = mConnections[iPort];
        if(null !is other) {
          averageLevel += other.getDiscreteComponent(iRes).getCurrentLevel();
          numConnections++;
        }
      }
    
      // REFACTOR Remove

      //     for(PortIterator portI = mPorts.begin(); portI != mPorts.end(); ++portI) {
      //       Port* currentPort = *portI;
      //       if(currentPort.isConnected()) {
      //         averageLevel += currentPort.getConnection().getHost().getDiscreteComponent(iRes).getCurrentLevel();
      //         numConnections++;
      //       }
      //     }

      if(0 == numConnections)
        averageLevel = 0;
      else
        averageLevel /= numConnections;
      flowComponent.setTargetLevel(averageLevel);
    }  
  }




private:
  Prototype mPrototype;  
  Station mStation;

  Vector mPosition;
  Quaternion mOrientation;
  
  bool mEnergyOnline;
  bool mThermalOnline;  

  float mFlowTotal[DiscreteResource.max + 1];

  DiscreteComponent mDiscreteComponents[DiscreteResource.max + 1];

  bool mProfitResource[DiscreteResource.max + 1];
  float mProfitCurrent;

  Module[] mConnections;
}
