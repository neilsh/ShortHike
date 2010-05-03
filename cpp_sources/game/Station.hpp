//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef STATION_HPP
#define STATION_HPP

class Module;
class Group;
class Port;
class Prototype;
class Mission;
class Location;
class RenderSystem;
class DiscreteComponent;

#include "DiscreteComponent.hpp"

typedef vector<pair<Module*,unsigned int> > PortConnectionList;


class Station
{
public:
  Station(Mission* mission);

  Mission* getMission() {return mMission;}

  void addModule(Module* newModule);
  void removeModule(Module* target);
  const set<Module*>& getModules() const {return mModules;}

  void connectModule(Module* newModule);
  void disconnectModule(Module* target);
  PortConnectionList findConnectedPorts(Prototype* prototype, const Matrix& transform, Module* ignoreModule = NULL);

  unsigned int getModuleCount() {return mModules.size();}
  unsigned int getEnergyOnlineCount() {return mEnergyOnlineCount;}
  unsigned int getThermalOnlineCount() {return mThermalOnlineCount;}
    
  Module* getFirstModule() {return *(mModules.begin());}

  Real getMass();
  float getEnergyProduction() {return mEnergyProduction;}
  float getEnergyConsumption() {return mEnergyConsumption;}
  float getThermalProduction() {return mThermalProduction;}
  float getThermalConsumption() {return mThermalConsumption;}    

  Real getTotalIncome() const {return mTotalIncome;}
  Real getTotalExpenses() const {return mTotalExpenses;}

  void updateSimulation();
  void advanceTurn();

  void stabilizeSimulation();
  bool isSimulationStable() {return mOnlineStable && mDiscreteStable;}
  void markSimulationUnstable();

  int getDiscreteIterations() {return mDiscreteIterations;}
  int getDiscreteIterationsType(DiscreteResource iRes) {return mDiscreteIterationsType[iRes];}
  float getDiscreteTimeAdvanceTrend() {return mDiscreteTimeAdvanceTrend;}

  DiscreteHeap& getDiscreteHeap();
private:
  virtual ~Station() {}

  void updateOnline();
  void updateOnlineModule(Module* modoule);

  Mission* mMission;
  set<Module*> mModules;

  bool mOnlineStable;
  bool mDiscreteStable;
  float mDiscreteTime;

  int mDiscreteIterations;
  float mDiscreteTimeAdvanceTrend;
  int mDiscreteIterationsType[DISCRETE_NUM];

  unsigned int mEnergyOnlineCount;
  float mEnergyProduction;
  float mEnergyConsumption;
  unsigned int mThermalOnlineCount;
  float mThermalProduction;
  float mThermalConsumption;

  Real mTotalIncome;
  Real mTotalExpenses;

  vector<DiscreteComponent*> mDESHeap;
};


#endif
