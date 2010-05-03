//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "Station.hpp"
#include "Aspects.hpp"
#include "DiscreteComponent.hpp"
#include "Group.hpp"
#include "Mission.hpp"
#include "Module.hpp"
#include "Port.hpp"
#include "Prototype.hpp"

#include "main/Main.hpp"
#include "rendering/Mesh.hpp"
//--------------------------------------------------------------------------------

Station::Station(Mission* mission)
  : mMission(mission),
    mOnlineStable(false), mDiscreteStable(false), mDiscreteIterations(0), mDiscreteTime(0),
    mEnergyProduction(0), mEnergyConsumption(0), mThermalProduction(0), mThermalConsumption(0),
    mEnergyOnlineCount(0), mThermalOnlineCount(0),
    mTotalIncome(0), mTotalExpenses(0)
{
}


//--------------------------------------------------------------------------------

const float DES_STABLE_TIME_ADVANCE = 1000;
const int DES_PASSES_PER_FRAME = 10;
const float MAX_DISCRETE_TIME = 1000000;

void
Station::markSimulationUnstable()
{
  if(mDiscreteTime > MAX_DISCRETE_TIME) {
    for(set<Module*>::const_iterator moduleI = mModules.begin(); moduleI != mModules.end(); ++moduleI) {
      Module* currentModule = *moduleI;
      for(int iDES = DISCRETE_INITIAL; iDES < DISCRETE_NUM; ++iDES) {
        DiscreteComponent* des = currentModule->getDiscreteComponent(iDES);
        des->resetTimeAdvance(mDiscreteTime);
      }
    }
    mDiscreteTime = 0;
  }  

  mOnlineStable = false;
  mDiscreteStable = false;
  mDiscreteIterations = 0;
  mDiscreteTimeAdvanceTrend = 0;

  for(int iDES = DISCRETE_INITIAL; iDES < DISCRETE_NUM; ++iDES)
    mDiscreteIterationsType[iDES] = 0;
}

void
Station::stabilizeSimulation()
{
  while(isSimulationStable() == false) {
    updateSimulation();
  }  
}

//--------------------------------------------------------------------------------

void
Station::updateSimulation()
{
  Prof(updateSimulation);
  
  if(0 == mModules.size()) return;

  if(!mOnlineStable) {
    updateOnline();
    mOnlineStable = true;
  }
  else if(!mDiscreteStable) {
    for(int pass = 0; pass < DES_PASSES_PER_FRAME; ++pass) { 
      mDiscreteIterations++;
      DiscreteComponent* imminent = NULL;
      float minNext = INFINITE_TIME_ADVANCE;
      {
        Prof(des_find_imminent);
        if(mDESHeap.size() == 0) {
          imminent = NULL;
        }
        else {
          imminent = mDESHeap[0];
          minNext = imminent->mNextUpdate;
        }
        if(minNext < mDiscreteTime) minNext = mDiscreteTime;
      }
      
      float nextTimeAdvance = minNext - mDiscreteTime;
      mDiscreteTimeAdvanceTrend = mDiscreteTimeAdvanceTrend * 0.95 + 0.05 * nextTimeAdvance;

      if(nextTimeAdvance > DES_STABLE_TIME_ADVANCE || imminent == NULL) {
        mDiscreteStable = true;
        return;
      }

      {
        Prof(des_update);
        mDiscreteTime = minNext;
        imminent->updateSimulation(mDiscreteTime);
        mDiscreteIterationsType[imminent->getIndex()]++;
      }
    }
  }  
}

// --------------------------------------------------------------------------------

void
Station::advanceTurn()
{
  mTotalIncome = 0;
  mTotalExpenses = 0;
  for(set<Module*>::const_iterator moduleI = mModules.begin(); moduleI != mModules.end(); ++moduleI) {
    Module* currentModule = *moduleI;
    currentModule->advanceTurn();
    mTotalIncome += currentModule->getProfit();
    mTotalExpenses += currentModule->getPrototype()->getSimFloat(SIM_COST_MAINTENANCE);
  }
}


// --------------------------------------------------------------------------------
// Push modules online in terms of energy and thermal control

bool
lessDependentProducer(Module* module1, Module* module2)
{
  float efficiency1;
  if(module1->getPrototype()->getSimFloat(SIM_ENERGY) > module1->getPrototype()->getSimFloat(SIM_THERMAL))
    efficiency1 = module1->getPrototype()->getSimFloat(SIM_ENERGY) / fabs(module1->getPrototype()->getSimFloat(SIM_THERMAL));
  else
    efficiency1 = module1->getPrototype()->getSimFloat(SIM_THERMAL) / fabs(module1->getPrototype()->getSimFloat(SIM_ENERGY));

  float efficiency2;
  if(module2->getPrototype()->getSimFloat(SIM_ENERGY) > module2->getPrototype()->getSimFloat(SIM_THERMAL))
    efficiency2 = module2->getPrototype()->getSimFloat(SIM_ENERGY) / fabs(module2->getPrototype()->getSimFloat(SIM_THERMAL));
  else
    efficiency2 = module2->getPrototype()->getSimFloat(SIM_THERMAL) / fabs(module2->getPrototype()->getSimFloat(SIM_ENERGY));
  return efficiency1 < efficiency2;
}

bool
lessConsumer(Module* module1, Module* module2)
{
  float order1 = 0;
  float order2 = 0;
  float step = 1;
  if(module1->getPrototype()->getSimFloat(SIM_ENERGY) < 0) order1 += step;
  if(module2->getPrototype()->getSimFloat(SIM_ENERGY) < 0) order2 += step;
  step = step * 2;
  if(module1->getPrototype()->getSimFloat(SIM_THERMAL) < 0) order1 += step;
  if(module2->getPrototype()->getSimFloat(SIM_THERMAL) < 0) order2 += step;

  return order1 < order2;
}

void
Station::updateOnlineModule(Module* module)
{
  if(module->isEnergyOnline()) {
    if(module->getPrototype()->getSimFloat(SIM_ENERGY) >= 0)
      mEnergyProduction += module->getPrototype()->getSimFloat(SIM_ENERGY);
    else
      mEnergyConsumption += module->getPrototype()->getSimFloat(SIM_ENERGY);
  }
  if(module->isThermalOnline()) {
    if(module->getPrototype()->getSimFloat(SIM_THERMAL) >= 0)
      mThermalProduction += module->getPrototype()->getSimFloat(SIM_THERMAL);
    else
      mThermalConsumption += module->getPrototype()->getSimFloat(SIM_THERMAL);
  }
}

// --------------------------------------------------------------------------------

void
Station::updateOnline()
{
  vector<Module*> dependentProducers;
  vector<Module*> consumers;
  mEnergyProduction = 0;
  mEnergyConsumption = 0;
  mThermalProduction = 0;
  mThermalConsumption = 0;

  // Sort everyone into their proper slot and turn on all independent producers
  for(set<Module*>::const_iterator moduleI = mModules.begin(); moduleI != mModules.end(); ++moduleI) {
    Module* currentModule = *moduleI;
    if(currentModule->getPrototype()->isIndependentlyOnline()) {
      currentModule->setOnline(true);
      updateOnlineModule(currentModule);
    }
    else if(currentModule->getPrototype()->getSimFloat(SIM_ENERGY) > 0 || currentModule->getPrototype()->getSimFloat(SIM_THERMAL) > 0) {
      dependentProducers.push_back(currentModule);
      currentModule->setOnline(false);
    }
    else {
      consumers.push_back(currentModule);
      currentModule->setOnline(false);
    }    
  }

  // Push as many dependent producers as possible online
  sort(dependentProducers.begin(), dependentProducers.end(), lessDependentProducer);
  for(vector<Module*>::const_iterator iModule = dependentProducers.begin(); iModule != dependentProducers.end(); ++iModule) {
    Module* currentModule = *iModule;
    // Thermal dependent
    if(currentModule->getPrototype()->getSimFloat(SIM_ENERGY) > 0) {
      if((currentModule->getPrototype()->getSimFloat(SIM_THERMAL) + mThermalConsumption + mThermalProduction) >= 0) {
        currentModule->setOnline(true);
        updateOnlineModule(currentModule);
      }      
    }
    // Energy dependent
    else {
      if((currentModule->getPrototype()->getSimFloat(SIM_ENERGY) + mEnergyConsumption + mEnergyProduction) >= 0) {
        currentModule->setOnline(true);
        updateOnlineModule(currentModule);
      }
    }
  }
  
  // Now push all consumers online in activation order
  sort(consumers.begin(), consumers.end(), lessConsumer);
  for(vector<Module*>::const_iterator iModule = consumers.begin(); iModule != consumers.end(); ++iModule) {
    Module* currentModule = *iModule;
    if((currentModule->getPrototype()->getSimFloat(SIM_ENERGY) + mEnergyConsumption + mEnergyProduction) >= 0) {
      currentModule->setEnergyOnline(true);
    }
    if((currentModule->getPrototype()->getSimFloat(SIM_THERMAL) + mThermalConsumption + mThermalProduction) >= 0) {
      currentModule->setThermalOnline(true);
    }
    updateOnlineModule(currentModule);
  }  

  mEnergyOnlineCount = 0;
  mThermalOnlineCount = 0;

  for(set<Module*>::const_iterator moduleI = mModules.begin(); moduleI != mModules.end(); ++moduleI) {
    Module* currentModule = *moduleI;
    if(!mMission->getAspects()->isEnergy())
      currentModule->setEnergyOnline(true);
    if(!mMission->getAspects()->isThermal())
      currentModule->setThermalOnline(true);
    if(currentModule->isEnergyOnline())
      mEnergyOnlineCount++;
    if(currentModule->isThermalOnline())
      mThermalOnlineCount++;
  }
}


//--------------------------------------------------------------------------------


void
Station::addModule(Module* newModule)
{
  mModules.insert(newModule);
  newModule->setStation(this);
  for(int iDES = DISCRETE_INITIAL; iDES < DISCRETE_NUM; ++iDES) {
    DiscreteComponent* des = newModule->getDiscreteComponent(iDES);
    unsigned int heapIndex = mDESHeap.size();
    des->mHeapIndex = heapIndex;
    mDESHeap.push_back(des);
    percolateDESHeap(mDESHeap, heapIndex);
  }

  connectModule(newModule);
//   rMain()->getMission()->doString("mission_event_module_add()");
}

#pragma warning(disable: 4100)
// Module*
// Station::addModule(string name, Port* connectionPort, Port* targetPort)
// {
// //   PrototypeManager& pManager = PrototypeManager::getSingleton();
  
// //   Module* newModule = pManager.createInstanceClass<Module>(getDataFile(), name);
// //   if(newModule == NULL) return NULL;
  
// //   addModule(newModule, connectionPort, targetPort);
// //   return newModule;
//   // REFACTOR: Change to new Module listing facility
//   return NULL;
// }

//--------------------------------------------------------------------------------

void
Station::removeModule(Module* target) 
{
//   rMain()->getMission()->doString("mission_event_module_remove()");

  disconnectModule(target);

  for(int iDES = DISCRETE_INITIAL; iDES < DISCRETE_NUM; ++iDES) {
    DiscreteComponent* des = target->getDiscreteComponent(iDES);
    unsigned int iHeap = des->mHeapIndex;
    mDESHeap[iHeap] = mDESHeap.back();
    mDESHeap.pop_back();
    percolateDESHeap(mDESHeap, iHeap);
  }

  target->setStation(NULL);
  mModules.erase(target);
  markSimulationUnstable();
}



//--------------------------------------------------------------------------------


void
Station::connectModule(Module* module)
{
  // Sanity check
//   if(NULL != targetPort && targetPort->isConnected()) return;

//   if(NULL != connectionPort && NULL != targetPort) {
//     Quaternion moduleOrientation;
//     Vector modulePosition;
//     module->findTransformAgainstPort(connectionPort, targetPort, modulePosition, moduleOrientation);
//     module->setPosition(modulePosition);
//     module->setOrientation(moduleOrientation);
//   }

  
  PortConnectionList connections = findConnectedPorts(module->getPrototype(), module->getTransform(), module);
  for(unsigned int iPort = 0; iPort < connections.size(); ++iPort) {
    if(connections[iPort].first != NULL)
      module->connectPort(iPort, connections[iPort].first, connections[iPort].second);
//   PortConnectionList connections = module->findConnectingPorts();
//   for(PortConnectionIterator iPortPair = connections.begin(); iPortPair != connections.end(); ++iPortPair) {
//     Port* myPort = (*iPortPair).first;
//     Port* otherPort = (*iPortPair).second;
// //     myPort->connect(otherPort);
//   }
  }
  
  module->preloadDES();
  markSimulationUnstable();
}

//--------------------------------------------------------------------------------

void
Station::disconnectModule(Module* target)
{
  target->disconnectAllPorts();
  markSimulationUnstable();
}

// --------------------------------------------------------------------------------

// PortConnectionList
// Module::findConnectingPorts(const set<Module*>& modules, const Vector& myPosition, const Quaternion& myOrientation) const
// {

PortConnectionList
Station::findConnectedPorts(Prototype* prototype, const Matrix& transform, Module* ignoreModule)
{
  PortConnectionList connections;  
  const PortList& myPorts = prototype->getPorts();
  connections.resize(myPorts.size());
  fill<PortConnectionList::iterator, pair<Module*, unsigned int> >(connections.begin(), connections.end(), pair<Module*, unsigned int>(NULL, 0));
  
  Vector myPosition = transform.getPosition();
  Quaternion myOrientation = transform.getOrientation();
  Real radius = prototype->getMesh()->getBoundingRadius();
  for(set<Module*>::const_iterator moduleI = mModules.begin(); moduleI != mModules.end(); ++moduleI) {
    Module* target = *moduleI;
    if(NULL != ignoreModule && target == ignoreModule) continue;

    Vector displacement = myPosition - target->getPosition();
    if(displacement.length() < (radius + target->getPrototype()->getMesh()->getBoundingRadius())) {

      const PortList& otherPorts = target->getPrototype()->getPorts();
      for(unsigned int iMyPort = 0; iMyPort < myPorts.size(); ++iMyPort) {
        for(unsigned int iOtherPort = 0; iOtherPort < otherPorts.size(); ++iOtherPort) {
          if(myPorts[iMyPort]->canConnect(otherPorts[iOtherPort], transform, target->getTransform())) {
            connections[iMyPort] = pair<Module*, unsigned int>(target, iOtherPort);
          }
        }
      }
      
//       for(PortIterator iMyPort = mPorts.begin(); iMyPort != mPorts.end(); ++iMyPort) {
//         Port* myPort = *iMyPort;
//         for(PortIterator iOtherPort = target->mPorts.begin(); iOtherPort != target->mPorts.end(); ++iOtherPort) {
//           Port* otherPort = *iOtherPort;
//           if(myPort->canConnect(otherPort, myPosition, myOrientation, target->getPosition(), target->getOrientation())) {
//             connections.insert(pair<Port*, Port*>(myPort, otherPort));
//           }
//         }
//       }

    }
  }
  return connections;
}

//--------------------------------------------------------------------------------
// This function assembles a group of modules into a single new module.

// const float MAGIC_ASSEMBLY_HARDWARE_COST = 0.95;
// const float MAGIC_ASSEMBLY_MAINTENANCE_COST = 0.90;
// const float MAGIC_ASSEMBLY_INITIAL_COST = 0.15;

// Module*
// Station::assembleModules(set<Module*> moduleParts)
// {
//   if(moduleParts.size() < 2) return NULL;
//   if(false == mMission->isSolvent()) return NULL;

//   Module* assembly = new Module("Assembly");
//   Module* firstModule = *(moduleParts.begin());

//   assembly->setIconName(firstModule->getIconName());
  
//   // Proper placement
//   Vector centerOfMass = Vector::ZERO;
//   float totalMass = 0;
//   for(set<Module*>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module* part = *iModule;
//     centerOfMass += part->getPosition() * part->getMass();
//     totalMass += part->getMass();
//   }
//   centerOfMass /= totalMass;
//   assembly->setPosition(centerOfMass);
//   assembly->setOrientation(Quaternion::IDENTITY);
//   assembly->setMass(totalMass);

//   // Add meshes
//   for(set<Module*>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module* part = *iModule;
//     Vector modulePosition = part->getPosition() - centerOfMass;
//     const vector<ModuleMesh*>& meshes = part->getMeshes();
//     for(vector<ModuleMesh*>::const_iterator iMesh = meshes.begin(); iMesh != meshes.end(); ++iMesh) {
//       ModuleMesh* moduleMesh = *iMesh;
//       Vector meshPosition = (part->getOrientation() * moduleMesh->position) + modulePosition;
//       Quaternion meshOrientation = part->getOrientation() * moduleMesh->orientation;
//       assembly->addMesh(moduleMesh->name, meshPosition, meshOrientation);
//     }
//   }
  
//   // Add proper ports
//   int portCount = 0;
//   for(set<Module*>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module* part = *iModule;
//     PortList ports = part->getPorts();
//     for(PortIterator iPort = ports.begin(); iPort != ports.end(); ++iPort) {
//       Port* port = *iPort;
//       if(port->isConnected() == false || moduleParts.find(port->getConnection()->getHost()) == moduleParts.end()) {
//         Vector portPosition = (part->getPosition() - centerOfMass) + (part->getOrientation() * port->getPosition());
//         Quaternion portOrientation = part->getOrientation() * port->getOrientation();
//         ostringstream ossPortName;
//         ossPortName << "port" << portCount++;
//         assembly->addPort(ossPortName.str(), portPosition, portOrientation);
//       }
//     }
//   }

  // copy simulation values
//   float flowResources[FLOW_NUM];
//   float exportResources[EXPORT_NUM];
//   float hardwareCost = 0;
//   float maintenanceExpenses = 0;
//   for(int iChannel = FLOW_INITIAL; iChannel < FLOW_NUM; ++iChannel) {
//     flowResources[iChannel] = 0;
//   }
//   for(int iExport = EXPORT_INITIAL; iExport < EXPORT_NUM; ++iExport) {
//     exportResources[iExport] = 0;
//   }
//   for(set<Module*>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module* part = *iModule;
//     Unit* unit = part->getUnit();
//     for(FlowResource iChannel = FLOW_INITIAL; iChannel < FLOW_NUM; ++iChannel) {
//       flowResources[iChannel] += unit->getInputOutputCapacity(iChannel);
//     }
//     for(ExportResource iExport = EXPORT_INITIAL; iExport < EXPORT_NUM; ++iExport) {
//       exportResources[iExport] += unit->getExportCapacity(iExport);
//     }
//     hardwareCost += unit->getHardwareCost();
//     maintenanceExpenses += unit->getMaintenanceExpenses();
//   }
//   for(FlowResource iChannel = FLOW_INITIAL; iChannel < FLOW_NUM; ++iChannel) {
//     assembly->getUnit()->setInputOutputCapacity(iChannel, flowResources[iChannel]);
//   }
//   for(ExportResource iExport = EXPORT_INITIAL; iExport < EXPORT_NUM; ++iExport) {
//     assembly->getUnit()->setExportCapacity(iExport, exportResources[iExport]);
//   }
//   assembly->getUnit()->setHardwareCost(hardwareCost * MAGIC_ASSEMBLY_HARDWARE_COST);
//   assembly->getUnit()->setMaintenanceExpenses(maintenanceExpenses * MAGIC_ASSEMBLY_MAINTENANCE_COST);

//   for(set<Module*>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module* part = *iModule;
//     removeModule(part);
//   }
//   addModule(assembly);

  //    SoundManager::playSoundEffect("orbit/docking");
  // FIXME: Add proper cost for assemblies
//   mMission->creditTransaction(-hardwareCost * MAGIC_ASSEMBLY_INITIAL_COST);

//   return assembly;
//   return NULL;
// }


//--------------------------------------------------------------------------------
// Separate the station out into a group

// Group*
// Station::groupFromStation(DataFile* groupDataFile)
// {
//   if(groupDataFile == NULL) return NULL;

//   Group* group = new Group(groupDataFile);
//   for(set<Module*>::const_iterator iModule = mModules.begin(); iModule != mModules.end(); ++iModule) {
//     Module* original = *iModule;
//     Module* clone = original->clone(groupDataFile)->castToClass<Module>();
//     // We need to deep copy only the ModuleParts as we aren't interested in connections .
//     vector<ModuleMesh*> parts = original->getMeshes();
//     for(vector<ModuleMesh*>::const_iterator iPart = parts.begin(); iPart != parts.end(); ++iPart) {
//       ModuleMesh* part = *iPart;
//       ModuleMesh* partClone = part->clone(groupDataFile)->castToClass<ModuleMesh>();
//       clone->addMesh(partClone);
//     }
//     group->addModule(clone);
//   }
//   return group;
// }


// void
// Station::insertGroup(Group* newGroup)
// {
//   set<Module*> modules = newGroup->getModules();
//   for(set<Module*>::const_iterator iModule = modules.begin(); iModule != modules.end(); ++iModule) {
//     Module* original = *iModule;
//     Module* clone = original->clone(getDataFile())->castToClass<Module>();
//     vector<ModuleMesh*> parts = original->getMeshes();
//     for(vector<ModuleMesh*>::const_iterator iPart = parts.begin(); iPart != parts.end(); ++iPart) {
//       ModuleMesh* part = *iPart;
//       ModuleMesh* partClone = part->clone(getDataFile())->castToClass<ModuleMesh>();
//       clone->addMesh(partClone);
//     }
//     addModule(clone);
//   }
// }


//--------------------------------------------------------------------------------
// Attach station to a specific SceneNode

// void
// Station::attachStation(SceneNode* rootNode)
// {
//   sceneNode = rootNode->createChildSceneNode();
//   CHECK("Root SceneNode returns valid handle", sceneNode != NULL);
//   sceneNode->setPosition(Vector::ZERO);
//   sceneNode->setOrientation(Quaternion::IDENTITY);
//   for(set<Module*>::const_iterator moduleI = mModules.begin(); moduleI != mModules.end(); ++moduleI) {
//     Module* currentModule = *moduleI;
//     currentModule->attachModule(rootNode);
//   }
// }

// void
// Station::detachStation()
// {
//   for(set<Module*>::const_iterator moduleI = mModules.begin(); moduleI != mModules.end(); ++moduleI) {
//     Module* currentModule = *moduleI;
//     currentModule->detachModule();
//   }
//   sceneNode->getParentSceneNode()->removeChild(sceneNode);
//   delete sceneNode;
//   sceneNode = NULL;
// }

//--------------------------------------------------------------------------------
// Show and hide ports depending on the mode

// void
// Station::setPortHandlesVisible(bool isVisible)
// {
//   for(set<Module*>::const_iterator moduleI = mModules.begin(); moduleI != mModules.end(); ++moduleI) {
//     Module* currentModule = *moduleI;
//     PortList ports = currentModule->getPorts();
//     for(PortList::const_iterator portI = ports.begin(); portI != ports.end(); ++portI) {
//       Port* currentPort = *portI;
//       currentPort->setHandleVisible(isVisible);
//     }
//   }
// }


//--------------------------------------------------------------------------------
// Finding general information about the station

Real
Station::getMass()
{
  Real mass = 0;
  for(set<Module*>::const_iterator moduleI = mModules.begin(); moduleI != mModules.end(); ++moduleI) {
    Module* currentModule = *moduleI;
    mass += currentModule->getPrototype()->getSimFloat(SIM_MASS);
  }
  return mass;
}

// --------------------------------------------------------------------------------

DiscreteHeap&
Station::getDiscreteHeap()
{
  return mDESHeap;
}
