//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "Module.hpp"
#include "DiscreteComponent.hpp"
#include "DiscreteDiffuse.hpp"
#include "DiscreteFlow.hpp"
#include "Mission.hpp"
#include "Part.hpp"
#include "Port.hpp"
#include "Prototype.hpp"
#include "Station.hpp"

#include "main/Main.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/Texture.hpp"

//--------------------------------------------------------------------------------
// FIXME:
// - Sense when the whole station is flipping and adjust all values accordingly


//--------------------------------------------------------------------------------

inline float flowFactor(DiscreteResource resource)
{
  if(DISCRETE_PASSENGERS == resource) return 0.15;
  if(DISCRETE_VISITORS == resource) return 0.3;
  if(DISCRETE_CREW == resource) return 0.2;
  if(DISCRETE_SERVICE == resource) return 0.3;
  if(DISCRETE_CARGO == resource) return 0.1;
  if(DISCRETE_COMMODITIES == resource) return 0.2;
  return 0;
}

//--------------------------------------------------------------------------------

Module::Module(Prototype* prototype)
  : mPrototype(prototype), mStation(NULL),
    position(Vector::ZERO), orientation(Quaternion::IDENTITY),
    mEnergyOnline(false), mThermalOnline(false),
    mProfitCurrent(0)
{
  mConnections.resize(prototype->getPorts().size());
  fill<vector<Module*>::iterator, Module*>(mConnections.begin(), mConnections.end(), NULL);

  mDiscreteComponents[DISCRETE_LIFESUPPORT] = new DiscreteDiffuse(this, DISCRETE_LIFESUPPORT);
  mDiscreteComponents[DISCRETE_STRESS] = new DiscreteDiffuse(this, DISCRETE_STRESS);
  
  mDiscreteComponents[DISCRETE_PASSENGERS] = new DiscreteFlow(this, DISCRETE_PASSENGERS);
  mDiscreteComponents[DISCRETE_VISITORS] = new DiscreteFlow(this, DISCRETE_VISITORS);
  mDiscreteComponents[DISCRETE_CREW] = new DiscreteFlow(this, DISCRETE_CREW);
  mDiscreteComponents[DISCRETE_CARGO] = new DiscreteFlow(this, DISCRETE_CARGO);
  mDiscreteComponents[DISCRETE_COMMODITIES] = new DiscreteFlow(this, DISCRETE_COMMODITIES);
  mDiscreteComponents[DISCRETE_SERVICE] = new DiscreteFlow(this, DISCRETE_SERVICE);
  
  for(int iRes = DISCRETE_INITIAL; iRes < DISCRETE_NUM; ++iRes) {
    mFlowTotal[iRes] = 0;
    mProfitResource[iRes] = false;
  }  
  
  setLifeSupportBase(mPrototype->getSimFloat(SIM_LIFESUPPORT_BASE));
  setFlowLocal(DISCRETE_PASSENGERS, mPrototype->getSimFloat(SIM_FLOW_PASSENGERS));
  setFlowLocal(DISCRETE_VISITORS, mPrototype->getSimFloat(SIM_FLOW_VISITORS));
  setFlowLocal(DISCRETE_CREW, mPrototype->getSimFloat(SIM_FLOW_CREW));
  setFlowLocal(DISCRETE_SERVICE, mPrototype->getSimFloat(SIM_FLOW_SERVICE));
  setFlowLocal(DISCRETE_CARGO, mPrototype->getSimFloat(SIM_FLOW_CARGO));
  setFlowLocal(DISCRETE_COMMODITIES, mPrototype->getSimFloat(SIM_FLOW_COMMODITIES));

  setProfitResource(DISCRETE_PASSENGERS, mPrototype->getSimBool(SIM_PROFIT_PASSENGERS));
  setProfitResource(DISCRETE_VISITORS, mPrototype->getSimBool(SIM_PROFIT_VISITORS));
  setProfitResource(DISCRETE_CREW, mPrototype->getSimBool(SIM_PROFIT_CREW));
  setProfitResource(DISCRETE_SERVICE, mPrototype->getSimBool(SIM_PROFIT_SERVICE));
  setProfitResource(DISCRETE_CARGO, mPrototype->getSimBool(SIM_PROFIT_CARGO));
  setProfitResource(DISCRETE_COMMODITIES, mPrototype->getSimBool(SIM_PROFIT_COMMODITIES));

  // DEBUG
  setFlowLocal(DISCRETE_PASSENGERS, static_cast<int>(fnrand() * 10 + 6));
  setFlowLocal(DISCRETE_VISITORS, static_cast<int>(fnrand() * 10 + 6));
  setFlowLocal(DISCRETE_CREW, static_cast<int>(fnrand() * 10 + 6));
  setFlowLocal(DISCRETE_SERVICE, static_cast<int>(fnrand() * 10 + 6));
  setFlowLocal(DISCRETE_CARGO, static_cast<int>(fnrand() * 10 + 6));
  setFlowLocal(DISCRETE_COMMODITIES, static_cast<int>(fnrand() * 10 + 6));
}


//--------------------------------------------------------------------------------


void
Module::setPosition(const Vector& _position)
{
  position = _position;
}


const Vector&
Module::getPosition() const
{
  return position;
}

const Vector&
Module::getWorldPosition() const
{
  return position;
}



void
Module::setOrientation(const Quaternion& _orientation)
{
  orientation = _orientation;
  orientation.normalize();
}


const Quaternion&
Module::getOrientation() const
{
  return orientation;
}


//--------------------------------------------------------------------------------

Matrix
Module::getTransform() const
{
  Matrix transform(getPosition(), getOrientation());
  return transform;
}


void
Module::setTransform(const Matrix& transform)
{
  setOrientation(transform.getOrientation());
  setPosition(transform.getPosition());
}


//--------------------------------------------------------------------------------

// REFACTOR: Move up to specific function for finding this matrix
#pragma warning(disable: 4100)
// void
// Module::rotate(const Matrix& unitToWorld, Degree angle)
// {
//   CHECK_FAIL("REFACTORED");
//   int connections = 0;
//   Port* jointPort = NULL;
//   for(PortIterator portI = mPorts.begin(); portI != mPorts.end(); ++portI) {
//     Port* currentPort = *portI;
//     if(currentPort->isConnected()) {
//       jointPort = currentPort;
//       connections++;
//     }
//   }
//   if(connections != 1) return;
  
//   Matrix portToUnit = jointPort->getTransform();
//   Matrix unitToPort = portToUnit.inverse();  
//   Matrix rotation(Quaternion(Vector(0, 0, 1), angle));
    
//   Matrix newTransform = unitToWorld * portToUnit * rotation * unitToPort;

//   setTransform(newTransform);
// }

// void
// Module::rotate(Degree angle)
// {
//   rotate(getTransform(), angle);
// }


bool
Module::canRotate() const
{
  int connections = 0;
  for(unsigned int iPort = 0; iPort < mConnections.size(); ++iPort) {
    if(NULL != mConnections[iPort])
      connections++;
  }
  return connections == 1;
}


//--------------------------------------------------------------------------------

// REFACTOR: This should be a separate function

// void
// Module::findTransformAgainstPort(Port* connectionPort, Port* targetPort, Vector& oModulePosition, Quaternion& oModuleOrientation, Degree iRotation) const
// {
//   Matrix portToModule(connectionPort->getPosition(), connectionPort->getOrientation());
//   Matrix moduleToPort = portToModule.inverse();

//   Matrix rotateModule(Vector::ZERO, Quaternion(Vector::UNIT_Z, iRotation));
//   Matrix flipPort(Vector::ZERO, Quaternion(Vector::UNIT_X, Degree(180)));
  
//   Matrix portToHost(targetPort->getPosition(), targetPort->getOrientation());
//   Matrix hostToStation(targetPort->getHost()->getPosition(), targetPort->getHost()->getOrientation());
  
//   Matrix moduleToStation = hostToStation * portToHost * flipPort * rotateModule * moduleToPort;
  
//   oModulePosition = moduleToStation.getPosition();
//   oModuleOrientation = moduleToStation.getOrientation();
//   oModuleOrientation.normalize();
// }

//--------------------------------------------------------------------------------

// PortConnectionList
// Module::findConnectingPorts() const
// {
//   if(NULL != mStation)
//     return findConnectingPorts(mStation->getModules(), position, orientation);
//   else
//     return PortConnectionList();
// }


// REFACTOR: This belongs probably better somewhere else? It's queried both by the station
// and by the building interface when placing a prototype. So it should take the prototype
// as argument?

// PortConnectionList
// Module::findConnectingPorts(const set<Module*>& modules, const Vector& myPosition, const Quaternion& myOrientation) const
// {
//   CHECK_FAIL("REFACTOR");
//   PortConnectionList connections;
// //   Real radius = mPrototype->getMesh()->getBoundingRadius();
// //   for(set<Module*>::const_iterator moduleI = modules.begin(); moduleI != modules.end(); ++moduleI) {
// //     Module* target = *moduleI;
// //     if(target == this) continue;

// //     Vector displacement = myPosition - target->getPosition();
// //     if(displacement.length() < (radius + target->getPrototype()->getMesh()->getBoundingRadius())) {
// //       for(PortIterator iMyPort = mPorts.begin(); iMyPort != mPorts.end(); ++iMyPort) {
// //         Port* myPort = *iMyPort;
// //         for(PortIterator iOtherPort = target->mPorts.begin(); iOtherPort != target->mPorts.end(); ++iOtherPort) {
// //           Port* otherPort = *iOtherPort;
// //           if(myPort->canConnect(otherPort, myPosition, myOrientation, target->getPosition(), target->getOrientation())) {
// //             connections.insert(pair<Port*, Port*>(myPort, otherPort));
// //           }
// //         }
// //       }
// //     }
// //   }
//   return connections;
// }



//--------------------------------------------------------------------------------
// Ports

void
Module::connectPort(unsigned int iMyPort, Module* otherModule, unsigned int iOtherPort)
{
  disconnectPort(iMyPort);
  mConnections[iMyPort] = otherModule;
  otherModule->disconnectPort(iOtherPort);
  otherModule->mConnections[iOtherPort] = this;
}


void
Module::disconnectPort(unsigned int iPort)
{
  Module* other = mConnections[iPort];
  if(NULL != other) {
    for(unsigned int iOtherPort = 0; iOtherPort < other->mConnections.size(); ++iOtherPort) {
      if(other->mConnections[iOtherPort] == this)
        other->mConnections[iOtherPort] = NULL;
    }      
  }
  mConnections[iPort] = NULL;
}


void
Module::disconnectAllPorts()
{
  for(unsigned int iPort = 0; iPort < mConnections.size(); ++iPort) {
    disconnectPort(iPort);
  }
}


Matrix
Module::getFullPortTransform(unsigned int portIndex)
{
  return mPrototype->getPortTransform(portIndex) * getTransform();
}



//--------------------------------------------------------------------------------

bool
Module::isOnline() const
{
  return isEnergyOnline() && isThermalOnline();
}

void
Module::setOnline(bool online)
{
  mEnergyOnline = online;
  mThermalOnline = online;
}

bool
Module::isEnergyOnline() const
{
  if(mPrototype->getSimFloat(SIM_ENERGY) >= 0) return true;
  return mEnergyOnline;
}

void
Module::setEnergyOnline(bool online)
{
  mEnergyOnline = online;
}

bool
Module::isThermalOnline() const
{
  if(mPrototype->getSimFloat(SIM_THERMAL) >= 0) return true;
  return mThermalOnline;
}

void
Module::setThermalOnline(bool online)
{
  mThermalOnline = online;
}


//--------------------------------------------------------------------------------

// void
// Module::setEnergy(float energy)
// {
//   mEnergy = energy;
//   if(mStation != NULL)
//     mStation->markSimulationUnstable();
// }


// void
// Module::setThermal(float thermal)
// {
//   mThermal = thermal;
//   if(mStation != NULL)
//     mStation->markSimulationUnstable();
// }

// bool
// Module::isIndependentlyOnline()
// {
//   return mEnergy >= 0 && mThermal >=0;
// }

// --------------------------------------------------------------------------------

float
Module::getLifeSupport()
{
  return mDiscreteComponents[DISCRETE_LIFESUPPORT]->getCurrentLevel();
}

float
Module::getLifeSupportBase()
{
  return static_cast<DiscreteDiffuse*>(mDiscreteComponents[DISCRETE_LIFESUPPORT])->getBaseLevel();
}


void
Module::setLifeSupportBase(float baseLifeSupport)
{
  DiscreteDiffuse* lsDiffuse = static_cast<DiscreteDiffuse*>(mDiscreteComponents[DISCRETE_LIFESUPPORT]);
  lsDiffuse->setBaseLevel(baseLifeSupport);
  lsDiffuse->setTargetLevel(baseLifeSupport);
}

// --------------------------------------------------------------------------------


float
Module::getStress()
{
  return mDiscreteComponents[DISCRETE_STRESS]->getCurrentLevel();
}

float
Module::getStressBase()
{
  return static_cast<DiscreteDiffuse*>(mDiscreteComponents[DISCRETE_STRESS])->getBaseLevel();
}


void
Module::setStressBase(float baseStress)
{
  static_cast<DiscreteDiffuse*>(mDiscreteComponents[DISCRETE_LIFESUPPORT])->setBaseLevel(baseStress);
}

// --------------------------------------------------------------------------------

float
Module::getDiscreteCurrent(DiscreteResource resource) const
{
  return getDiscreteComponent(resource)->getCurrentLevel();
}

float
Module::getDiscreteTarget(DiscreteResource resource) const
{
  return getDiscreteComponent(resource)->getTargetLevel();
}

// --------------------------------------------------------------------------------

float
Module::getFlowLocal(DiscreteResource resource)
{
  return static_cast<DiscreteFlow*>(getDiscreteComponent(resource))->getLocalLevel() / flowFactor(resource);
}

void
Module::setFlowLocal(DiscreteResource resource, float value)
{
  DiscreteFlow* flowComponent = static_cast<DiscreteFlow*>(getDiscreteComponent(resource));
  flowComponent->setLocalLevelMax(value * flowFactor(resource));

  if(value < 0)
    flowComponent->setLocalLevelCurrent(1);
  else
    flowComponent->setLocalLevelCurrent(0);
}


float
Module::getFlowTotal() const
{
  float totalFlow = 0;
  for(int iRes = DISCRETE_INITIAL; iRes < DISCRETE_NUM; ++iRes)
    totalFlow += mFlowTotal[iRes];
  return totalFlow;
}

void
Module::updateFlowTotal(float discreteTime, DiscreteResource resource, float flow)
{
  mFlowTotal[resource] = flow;

  float utilization = 0;
  if(mPrototype->getSimFloat(SIM_FLOW_CAPACITY) > 0)
    utilization = getFlowTotal() / mPrototype->getSimFloat(SIM_FLOW_CAPACITY);
  utilization = clamp<float>(utilization, 0, 1);
  float stress = utilization * utilization * 5;
  if(stress > 10) stress = 10;

  DiscreteDiffuse* stressComponent = static_cast<DiscreteDiffuse*>(getDiscreteComponent(DISCRETE_STRESS));
  stressComponent->setBaseLevel(stress);
  stressComponent->updateTimeAdvance(discreteTime);
}



//--------------------------------------------------------------------------------

void
Module::advanceTurn()
{
  // Find minimum performance input
  float minimumPerformance = 1;
  for(int iRes = DISCRETE_FLOW_INITIAL; iRes < DISCRETE_NUM; ++iRes) {
    DiscreteFlow* flowComponent = static_cast<DiscreteFlow*>(getDiscreteComponent(iRes));
    if(flowComponent->getLocalLevelMax() < 0) {
      flowComponent->setLocalLevelCurrent(1);
      float currentPerformance = flowComponent->getCurrentPerformance();
      if(currentPerformance < minimumPerformance)
        minimumPerformance = currentPerformance;
    }
  }

  // Set all output to min performance
  for(int iRes = DISCRETE_FLOW_INITIAL; iRes < DISCRETE_NUM; ++iRes) {
    DiscreteFlow* flowComponent = static_cast<DiscreteFlow*>(getDiscreteComponent(iRes));
    if(flowComponent->getLocalLevelMax() >= 0) {
      flowComponent->setLocalLevelCurrent(minimumPerformance);
    }
  }
  
  // Find profit
  float minProfitPerformance = 1;
  if(mProfitResource[DISCRETE_LIFESUPPORT]) {
    float lsPerformance = getLifeSupport() / getLifeSupportBase() * 0.1;
    if(lsPerformance < minProfitPerformance)
      minProfitPerformance = lsPerformance;
  }
  for(int iRes = DISCRETE_FLOW_INITIAL; iRes < DISCRETE_NUM; ++iRes) {
    if(mProfitResource[iRes]) {
      DiscreteFlow* flowComponent = static_cast<DiscreteFlow*>(getDiscreteComponent(iRes));
      float flowPerformance = flowComponent->getCurrentPerformance();
      if(flowPerformance < minProfitPerformance)
        minProfitPerformance = flowPerformance;
    }
  }
  mProfitCurrent = minProfitPerformance * mPrototype->getSimFloat(SIM_PROFIT_MAX);
}


//--------------------------------------------------------------------------------

DiscreteComponent*
Module::getDiscreteComponent(int index) const
{
  CHECK("Valid DES index", index >= DISCRETE_INITIAL && index < DISCRETE_NUM);
  return mDiscreteComponents[index];
}

void
Module::preloadDES()
{
  for(int iRes = DISCRETE_FLOW_INITIAL; iRes < DISCRETE_NUM; ++iRes) {
    DiscreteFlow* flowComponent = static_cast<DiscreteFlow*>(getDiscreteComponent(iRes));
    float averageLevel = 0;
    int numConnections = 0;

    for(unsigned int iPort = 0; iPort < mConnections.size(); ++iPort) {
      Module* other = mConnections[iPort];
      if(NULL != other) {
        averageLevel += other->getDiscreteComponent(iRes)->getCurrentLevel();
        numConnections++;
      }
    }
    
    // REFACTOR Remove

//     for(PortIterator portI = mPorts.begin(); portI != mPorts.end(); ++portI) {
//       Port* currentPort = *portI;
//       if(currentPort->isConnected()) {
//         averageLevel += currentPort->getConnection()->getHost()->getDiscreteComponent(iRes)->getCurrentLevel();
//         numConnections++;
//       }
//     }

    if(0 == numConnections)
      averageLevel = 0;
    else
      averageLevel /= numConnections;
    flowComponent->setTargetLevel(averageLevel);
  }  
}








