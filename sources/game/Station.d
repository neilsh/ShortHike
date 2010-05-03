//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Module;
private import Port;
private import Mission;
private import Prototype;
private import DiscreteComponent;

// typedef vector<pair<Module*,uint> > PortConnectionList;

struct PortConnection
{
  Module mod = null;
  int port = 0;
}



class Station
{
public:
  Mission getMission() {return mMission;}

  Module[] getModules() {return mModules.keys;}


  uint getModuleCount() {return mModules.length;}
  uint getEnergyOnlineCount() {return mEnergyOnlineCount;}
  uint getThermalOnlineCount() {return mThermalOnlineCount;}
    
  Module getFirstModule() {return mModules.keys[0];}

  float getEnergyProduction() {return mEnergyProduction;}
  float getEnergyConsumption() {return mEnergyConsumption;}
  float getThermalProduction() {return mThermalProduction;}
  float getThermalConsumption() {return mThermalConsumption;}    

  float getTotalIncome() {return mTotalIncome;}
  float getTotalExpenses() {return mTotalExpenses;}
  bool isSimulationStable() {return cast(bool)(mOnlineStable && mDiscreteStable);}

  int getDiscreteIterations() {return mDiscreteIterations;}
  int getDiscreteIterationsType(DiscreteResource iRes) {return mDiscreteIterationsType[iRes];}
  float getDiscreteTimeAdvanceTrend() {return mDiscreteTimeAdvanceTrend;}

//--------------------------------------------------------------------------------

this(Mission mission)
{
  mMission = mission;
  mOnlineStable = false;
  mDiscreteStable = false;
  mDiscreteIterations = 0;
  mDiscreteTime = 0;
  mEnergyProduction = 0;
  mEnergyConsumption = 0;
  mThermalProduction = 0;
  mThermalConsumption = 0;
  mEnergyOnlineCount = 0;
  mThermalOnlineCount = 0;
  mTotalIncome = 0;
  mTotalExpenses = 0;
}


//--------------------------------------------------------------------------------

const float DES_STABLE_TIME_ADVANCE = 1000;
const int DES_PASSES_PER_FRAME = 10;
const float MAX_DISCRETE_TIME = 1000000;

void
markSimulationUnstable()
{
  if(mDiscreteTime > MAX_DISCRETE_TIME) {
    for(int iModule = 0; iModule < mModules.keys.length; ++iModule) {
      Module currentModule = mModules.keys[iModule];
      for(int iDES = DiscreteResource.min; iDES <= DiscreteResource.max; ++iDES) {
        DiscreteComponent des = currentModule.getDiscreteComponent(iDES);
        des.resetTimeAdvance(mDiscreteTime);
      }
    }
    mDiscreteTime = 0;
  }  

  mOnlineStable = false;
  mDiscreteStable = false;
  mDiscreteIterations = 0;
  mDiscreteTimeAdvanceTrend = 0;

  for(int iDES = DiscreteResource.min; iDES <= DiscreteResource.max; ++iDES)
    mDiscreteIterationsType[iDES] = 0;
}

void
stabilizeSimulation()
{
  while(isSimulationStable() == false) {
    updateSimulation();
  }  
}

//--------------------------------------------------------------------------------

void
updateSimulation()
{
//   Prof(updateSimulation);
  
  if(0 == mModules.keys.length) return;

  if(!mOnlineStable) {
    updateOnline();
    mOnlineStable = true;
  }
  else if(!mDiscreteStable) {
    for(int pass = 0; pass < DES_PASSES_PER_FRAME; ++pass) { 
      mDiscreteIterations++;
      DiscreteComponent imminent = null;
      float minNext = INFINITE_TIME_ADVANCE;
      {
//         Prof(des_find_imminent);
        if(mDESHeap.length == 0) {
          imminent = null;
        }
        else {
          imminent = mDESHeap[0];
          minNext = imminent.mNextUpdate;
        }
        if(minNext < mDiscreteTime) minNext = mDiscreteTime;
      }
      
      float nextTimeAdvance = minNext - mDiscreteTime;
      mDiscreteTimeAdvanceTrend = mDiscreteTimeAdvanceTrend * 0.95 + 0.05 * nextTimeAdvance;

      if(nextTimeAdvance > DES_STABLE_TIME_ADVANCE || imminent == null) {
        mDiscreteStable = true;
        return;
      }

      {
//         Prof(des_update);
        mDiscreteTime = minNext;
        imminent.updateSimulation(mDiscreteTime);
        mDiscreteIterationsType[imminent.getIndex()]++;
      }
    }
  }  
}

// --------------------------------------------------------------------------------

void
advanceTurn()
{
  mTotalIncome = 0;
  mTotalExpenses = 0;
  for(int iModule = 0; iModule < mModules.keys.length; ++iModule) {
    Module currentModule = mModules.keys[iModule];
    currentModule.advanceTurn();
    mTotalIncome += currentModule.getProfit();
    mTotalExpenses += currentModule.getPrototype().getSimFloat(SimFloat.COST_MAINTENANCE);
  }
}


// --------------------------------------------------------------------------------
// Push modules online in terms of energy and thermal control

bool
lessDependentProducer(Module module1, Module module2)
{
  float efficiency1;
  if(module1.getPrototype().getSimFloat(SimFloat.ENERGY) > module1.getPrototype().getSimFloat(SimFloat.THERMAL))
    efficiency1 = module1.getPrototype().getSimFloat(SimFloat.ENERGY) / fabs(module1.getPrototype().getSimFloat(SimFloat.THERMAL));
  else
    efficiency1 = module1.getPrototype().getSimFloat(SimFloat.THERMAL) / fabs(module1.getPrototype().getSimFloat(SimFloat.ENERGY));

  float efficiency2;
  if(module2.getPrototype().getSimFloat(SimFloat.ENERGY) > module2.getPrototype().getSimFloat(SimFloat.THERMAL))
    efficiency2 = module2.getPrototype().getSimFloat(SimFloat.ENERGY) / fabs(module2.getPrototype().getSimFloat(SimFloat.THERMAL));
  else
    efficiency2 = module2.getPrototype().getSimFloat(SimFloat.THERMAL) / fabs(module2.getPrototype().getSimFloat(SimFloat.ENERGY));
  return cast(bool)(efficiency1 < efficiency2);
}

bool
lessConsumer(Module module1, Module module2)
{
  float order1 = 0;
  float order2 = 0;
  float step = 1;
  if(module1.getPrototype().getSimFloat(SimFloat.ENERGY) < 0) order1 += step;
  if(module2.getPrototype().getSimFloat(SimFloat.ENERGY) < 0) order2 += step;
  step = step * 2;
  if(module1.getPrototype().getSimFloat(SimFloat.THERMAL) < 0) order1 += step;
  if(module2.getPrototype().getSimFloat(SimFloat.THERMAL) < 0) order2 += step;

  return cast(bool)(order1 < order2);
}

void
updateOnlineModule(Module mod)
{
  if(mod.isEnergyOnline()) {
    if(mod.getPrototype().getSimFloat(SimFloat.ENERGY) >= 0)
      mEnergyProduction += mod.getPrototype().getSimFloat(SimFloat.ENERGY);
    else
      mEnergyConsumption += mod.getPrototype().getSimFloat(SimFloat.ENERGY);
  }
  if(mod.isThermalOnline()) {
    if(mod.getPrototype().getSimFloat(SimFloat.THERMAL) >= 0)
      mThermalProduction += mod.getPrototype().getSimFloat(SimFloat.THERMAL);
    else
      mThermalConsumption += mod.getPrototype().getSimFloat(SimFloat.THERMAL);
  }
}

// --------------------------------------------------------------------------------

void
updateOnline()
{
  Module[] dependentProducers;
  Module[] consumers;
  mEnergyProduction = 0;
  mEnergyConsumption = 0;
  mThermalProduction = 0;
  mThermalConsumption = 0;

  // Sort everyone into their proper slot and turn on all independent producers
  for(int iModule = 0; iModule < mModules.keys.length; ++iModule) {
    Module currentModule = mModules.keys[iModule];
    if(currentModule.getPrototype().isIndependentlyOnline()) {
      currentModule.setOnline(true);
      updateOnlineModule(currentModule);
    }
    else if(currentModule.getPrototype().getSimFloat(SimFloat.ENERGY) > 0 || currentModule.getPrototype().getSimFloat(SimFloat.THERMAL) > 0) {
      dependentProducers ~= currentModule;
      currentModule.setOnline(false);
    }
    else {
      consumers ~= currentModule;
      currentModule.setOnline(false);
    }    
  }

  // Push as many dependent producers as possible online
  // DEBUG: Sort order
  //  sort(dependentProducers.begin(), dependentProducers.end(), lessDependentProducer);
  for(int iModule = 0; iModule < dependentProducers.length; ++iModule) {
    Module currentModule = dependentProducers[iModule];
    // Thermal dependent
    if(currentModule.getPrototype().getSimFloat(SimFloat.ENERGY) > 0) {
      if((currentModule.getPrototype().getSimFloat(SimFloat.THERMAL) + mThermalConsumption + mThermalProduction) >= 0) {
        currentModule.setOnline(true);
        updateOnlineModule(currentModule);
      }      
    }
    // Energy dependent
    else {
      if((currentModule.getPrototype().getSimFloat(SimFloat.ENERGY) + mEnergyConsumption + mEnergyProduction) >= 0) {
        currentModule.setOnline(true);
        updateOnlineModule(currentModule);
      }
    }
  }
  
  // Now push all consumers online in activation order
  // DEBUG: Sort order
//   sort(consumers.begin(), consumers.end(), lessConsumer);
  for(int iModule = 0; iModule < consumers.length; ++iModule) {
    Module currentModule = consumers[iModule];
    if((currentModule.getPrototype().getSimFloat(SimFloat.ENERGY) + mEnergyConsumption + mEnergyProduction) >= 0) {
      currentModule.setEnergyOnline(true);
    }
    if((currentModule.getPrototype().getSimFloat(SimFloat.THERMAL) + mThermalConsumption + mThermalProduction) >= 0) {
      currentModule.setThermalOnline(true);
    }
    updateOnlineModule(currentModule);
  }  

  mEnergyOnlineCount = 0;
  mThermalOnlineCount = 0;

  for(int iModule = 0; iModule < mModules.keys.length; ++iModule) {
    Module currentModule = mModules.keys[iModule];
    if(!mMission.isAspect(Aspect.ENERGY))
      currentModule.setEnergyOnline(true);
    if(!mMission.isAspect(Aspect.THERMAL))
      currentModule.setThermalOnline(true);
    if(currentModule.isEnergyOnline())
      mEnergyOnlineCount++;
    if(currentModule.isThermalOnline())
      mThermalOnlineCount++;
  }
}


//--------------------------------------------------------------------------------


void
addModule(Module newModule)
{
  mModules[newModule] = true;
  newModule.setStation(this);
  for(int iDES = DiscreteResource.min; iDES <= DiscreteResource.max; ++iDES) {
    DiscreteComponent des = newModule.getDiscreteComponent(iDES);
    uint heapIndex = mDESHeap.length;
    des.mHeapIndex = heapIndex;
    mDESHeap ~= des;
    percolateDESHeap(mDESHeap, heapIndex);
  }

  connectModule(newModule);
//   rMain().getMission().doString("mission_event_module_add()");
}

// Module
// addModule(string name, Port* connectionPort, Port* targetPort)
// {
// //   PrototypeManager& pManager = PrototypeManager::getSingleton();
  
// //   Module newModule = pManager.createInstanceClass<Module>(getDataFile(), name);
// //   if(newModule == null) return null;
  
// //   addModule(newModule, connectionPort, targetPort);
// //   return newModule;
//   // REFACTOR: Change to new Module listing facility
//   return null;
// }

//--------------------------------------------------------------------------------

void
removeModule(Module target) 
{
//   rMain().getMission().doString("mission_event_module_remove()");

  disconnectModule(target);

  for(int iDES = DiscreteResource.min; iDES <= DiscreteResource.max; ++iDES) {
    DiscreteComponent des = target.getDiscreteComponent(iDES);
    uint iHeap = des.mHeapIndex;
    mDESHeap[iHeap] = mDESHeap[mDESHeap.length - 1];
    mDESHeap[iHeap].mHeapIndex = iHeap;
    mDESHeap.length = mDESHeap.length - 1;
    if(iHeap < mDESHeap.length)
      percolateDESHeap(mDESHeap, iHeap);
  }

  target.setStation(null);
  mModules.remove(target);
  markSimulationUnstable();
}



//--------------------------------------------------------------------------------


void
connectModule(Module mod)
{
  // Sanity check
//   if(null !is targetPort && targetPort.isConnected()) return;

//   if(null !is connectionPort && null !is targetPort) {
//     Quaternion moduleOrientation;
//     Vector modulePosition;
//     module.findTransformAgainstPort(connectionPort, targetPort, modulePosition, moduleOrientation);
//     module.setPosition(modulePosition);
//     module.setOrientation(moduleOrientation);
//   }

  
  PortConnection[] connections = findConnectedPorts(mod.getPrototype(), mod.getTransform(), mod);
  for(uint iPort = 0; iPort < connections.length; ++iPort) {
    if(connections[iPort].mod !is null)
      mod.connectPort(iPort, connections[iPort].mod, connections[iPort].port);
//   PortConnection[] connections = mod.findConnectingPorts();
//   for(PortConnectionIterator iPortPair = connections.begin(); iPortPair != connections.end(); ++iPortPair) {
//     Port* myPort = (*iPortPair).first;
//     Port* otherPort = (*iPortPair).second;
// //     myPort.connect(otherPort);
//   }
  }
  
  mod.preloadDES();
  markSimulationUnstable();
}

//--------------------------------------------------------------------------------

void
disconnectModule(Module target)
{
  target.disconnectAllPorts();
  markSimulationUnstable();
}

// --------------------------------------------------------------------------------


PortConnection[] findConnectedPorts(Prototype prototype, Matrix transform, Module ignoreModule = null)
{
  PortConnection[] connections;  
  Port[] myPorts = prototype.getPorts();
  connections.length = myPorts.length;

  Vector myPosition = transform.getPosition();
  Quaternion myOrientation = transform.getOrientation();
  float radius = prototype.getMesh().getBoundingRadius();
  for(int iModule = 0; iModule < mModules.keys.length; ++iModule) {
    Module target = mModules.keys[iModule];
    if(null !is ignoreModule && target == ignoreModule) continue;

    Vector displacement = myPosition - target.getPosition();
    if(displacement.length() < (radius + target.getPrototype().getMesh().getBoundingRadius())) {

      Port[] otherPorts = target.getPrototype().getPorts();
      for(uint iMyPort = 0; iMyPort < myPorts.length; ++iMyPort) {
        for(uint iOtherPort = 0; iOtherPort < otherPorts.length; ++iOtherPort) {
          if(myPorts[iMyPort].canConnect(otherPorts[iOtherPort], transform, target.getTransform())) {
            connections[iMyPort].mod = target;
            connections[iMyPort].port = iOtherPort;
          }
        }
      }
      
//       for(PortIterator iMyPort = mPorts.begin(); iMyPort != mPorts.end(); ++iMyPort) {
//         Port* myPort = *iMyPort;
//         for(PortIterator iOtherPort = target.mPorts.begin(); iOtherPort != target.mPorts.end(); ++iOtherPort) {
//           Port* otherPort = *iOtherPort;
//           if(myPort.canConnect(otherPort, myPosition, myOrientation, target.getPosition(), target.getOrientation())) {
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

// Module
// assembleModules(set<Module> moduleParts)
// {
//   if(moduleParts.length < 2) return null;
//   if(false == mMission.isSolvent()) return null;

//   Module assembly = new Module("Assembly");
//   Module firstModule = *(moduleParts.begin());

//   assembly.setIconName(firstModule.getIconName());
  
//   // Proper placement
//   Vector centerOfMass = Vector::ZERO;
//   float totalMass = 0;
//   for(set<Module>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module part = mModules[iModule];
//     centerOfMass += part.getPosition() * part.getMass();
//     totalMass += part.getMass();
//   }
//   centerOfMass /= totalMass;
//   assembly.setPosition(centerOfMass);
//   assembly.setOrientation(Quaternion::IDENTITY);
//   assembly.setMass(totalMass);

//   // Add meshes
//   for(set<Module>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module part = mModules[iModule];
//     Vector modulePosition = part.getPosition() - centerOfMass;
//     const vector<ModuleMesh*>& meshes = part.getMeshes();
//     for(vector<ModuleMesh*>::const_iterator iMesh = meshes.begin(); iMesh != meshes.end(); ++iMesh) {
//       ModuleMesh* moduleMesh = *iMesh;
//       Vector meshPosition = (part.getOrientation() * moduleMesh.position) + modulePosition;
//       Quaternion meshOrientation = part.getOrientation() * moduleMesh.orientation;
//       assembly.addMesh(moduleMesh.name, meshPosition, meshOrientation);
//     }
//   }
  
//   // Add proper ports
//   int portCount = 0;
//   for(set<Module>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module part = mModules[iModule];
//     PortList ports = part.getPorts();
//     for(PortIterator iPort = ports.begin(); iPort != ports.end(); ++iPort) {
//       Port* port = *iPort;
//       if(port.isConnected() == false || moduleParts.find(port.getConnection().getHost()) == moduleParts.end()) {
//         Vector portPosition = (part.getPosition() - centerOfMass) + (part.getOrientation() * port.getPosition());
//         Quaternion portOrientation = part.getOrientation() * port.getOrientation();
//         ostringstream ossPortName;
//         ossPortName << "port" << portCount++;
//         assembly.addPort(ossPortName.str(), portPosition, portOrientation);
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
//   for(set<Module>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module part = mModules[iModule];
//     Unit* unit = part.getUnit();
//     for(FlowResource iChannel = FLOW_INITIAL; iChannel < FLOW_NUM; ++iChannel) {
//       flowResources[iChannel] += unit.getInputOutputCapacity(iChannel);
//     }
//     for(ExportResource iExport = EXPORT_INITIAL; iExport < EXPORT_NUM; ++iExport) {
//       exportResources[iExport] += unit.getExportCapacity(iExport);
//     }
//     hardwareCost += unit.getHardwareCost();
//     maintenanceExpenses += unit.getMaintenanceExpenses();
//   }
//   for(FlowResource iChannel = FLOW_INITIAL; iChannel < FLOW_NUM; ++iChannel) {
//     assembly.getUnit().setInputOutputCapacity(iChannel, flowResources[iChannel]);
//   }
//   for(ExportResource iExport = EXPORT_INITIAL; iExport < EXPORT_NUM; ++iExport) {
//     assembly.getUnit().setExportCapacity(iExport, exportResources[iExport]);
//   }
//   assembly.getUnit().setHardwareCost(hardwareCost * MAGIC_ASSEMBLY_HARDWARE_COST);
//   assembly.getUnit().setMaintenanceExpenses(maintenanceExpenses * MAGIC_ASSEMBLY_MAINTENANCE_COST);

//   for(set<Module>::const_iterator iModule = moduleParts.begin(); iModule != moduleParts.end(); ++iModule) {
//     Module part = mModules[iModule];
//     removeModule(part);
//   }
//   addModule(assembly);

  //    SoundManager::playSoundEffect("orbit/docking");
  // FIXME: Add proper cost for assemblies
//   mMission.creditTransaction(-hardwareCost * MAGIC_ASSEMBLY_INITIAL_COST);

//   return assembly;
//   return null;
// }


//--------------------------------------------------------------------------------
// Separate the station out into a group

// Group*
// groupFromStation(DataFile* groupDataFile)
// {
//   if(groupDataFile == null) return null;

//   Group* group = new Group(groupDataFile);
//   for(set<Module>::const_iterator iModule = mModules.begin(); iModule != mModules.end(); ++iModule) {
//     Module original = mModules[iModule];
//     Module clone = original.clone(groupDataFile).castToClass<Module>();
//     // We need to deep copy only the ModuleParts as we aren't interested in connections .
//     vector<ModuleMesh*> parts = original.getMeshes();
//     for(vector<ModuleMesh*>::const_iterator iPart = parts.begin(); iPart != parts.end(); ++iPart) {
//       ModuleMesh* part = *iPart;
//       ModuleMesh* partClone = part.clone(groupDataFile).castToClass<ModuleMesh>();
//       clone.addMesh(partClone);
//     }
//     group.addModule(clone);
//   }
//   return group;
// }


// void
// insertGroup(Group* newGroup)
// {
//   set<Module> modules = newGroup.getModules();
//   for(set<Module>::const_iterator iModule = modules.begin(); iModule != modules.end(); ++iModule) {
//     Module original = mModules[iModule];
//     Module clone = original.clone(getDataFile()).castToClass<Module>();
//     vector<ModuleMesh*> parts = original.getMeshes();
//     for(vector<ModuleMesh*>::const_iterator iPart = parts.begin(); iPart != parts.end(); ++iPart) {
//       ModuleMesh* part = *iPart;
//       ModuleMesh* partClone = part.clone(getDataFile()).castToClass<ModuleMesh>();
//       clone.addMesh(partClone);
//     }
//     addModule(clone);
//   }
// }


//--------------------------------------------------------------------------------
// Attach station to a specific SceneNode

// void
// attachStation(SceneNode* rootNode)
// {
//   sceneNode = rootNode.createChildSceneNode();
//   CHECK("Root SceneNode returns valid handle", sceneNode != null);
//   sceneNode.setPosition(Vector::ZERO);
//   sceneNode.setOrientation(Quaternion::IDENTITY);
//   for(set<Module>::const_iterator iModule = mModules.begin(); iModule != mModules.end(); ++iModule) {
//     Module currentModule = mModules[iModule];
//     currentModule.attachModule(rootNode);
//   }
// }

// void
// detachStation()
// {
//   for(set<Module>::const_iterator iModule = mModules.begin(); iModule != mModules.end(); ++iModule) {
//     Module currentModule = mModules[iModule];
//     currentModule.detachModule();
//   }
//   sceneNode.getParentSceneNode().removeChild(sceneNode);
//   delete sceneNode;
//   sceneNode = null;
// }

//--------------------------------------------------------------------------------
// Show and hide ports depending on the mode

// void
// setPortHandlesVisible(bool isVisible)
// {
//   for(set<Module>::const_iterator iModule = mModules.begin(); iModule != mModules.end(); ++iModule) {
//     Module currentModule = mModules[iModule];
//     PortList ports = currentModule.getPorts();
//     for(PortList::const_iterator portI = ports.begin(); portI != ports.end(); ++portI) {
//       Port* currentPort = *portI;
//       currentPort.setHandleVisible(isVisible);
//     }
//   }
// }


//--------------------------------------------------------------------------------
// Finding general information about the station

float
getMass()
{
  float mass = 0;
  for(int iModule = 0; iModule < mModules.keys.length; ++iModule) {
    Module currentModule = mModules.keys[iModule];
    mass += currentModule.getPrototype().getSimFloat(SimFloat.MASS);
  }
  return mass;
}

// --------------------------------------------------------------------------------

DiscreteComponent[]
getDiscreteHeap()
{
  return mDESHeap;
}

private:
  ~this() {}

  Mission mMission;
  bool[Module] mModules;

  bool mOnlineStable;
  bool mDiscreteStable;
  float mDiscreteTime;

  int mDiscreteIterations;
  float mDiscreteTimeAdvanceTrend;
  int mDiscreteIterationsType[DiscreteResource.max + 1];

  uint mEnergyOnlineCount;
  float mEnergyProduction;
  float mEnergyConsumption;
  uint mThermalOnlineCount;
  float mThermalProduction;
  float mThermalConsumption;

  float mTotalIncome;
  float mTotalExpenses;

  DiscreteComponent[] mDESHeap;
}
