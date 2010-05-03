//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef MODULE_HPP
#define MODULE_HPP

class Station;
class Part;
class Port;
class Module;
class DiscreteComponent;
class Mesh;
class RenderSystem;
class Prototype;

typedef vector<Module*> ConnectionList;

inline wstring toString(DiscreteResource resource)
{
  if(resource == DISCRETE_LIFESUPPORT)
    return L"LifeSupport";
  else if(resource == DISCRETE_STRESS)
    return L"Stress";
  else if(resource == DISCRETE_PASSENGERS)
    return L"Passengers";
  else if(resource == DISCRETE_VISITORS)
    return L"Visitors";
  else if(resource == DISCRETE_CREW)
    return L"Crew";
  else if(resource == DISCRETE_CARGO)
    return L"Cargo";
  else if(resource == DISCRETE_COMMODITIES)
    return L"Commodities";
  else if(resource == DISCRETE_SERVICE)
    return L"Service";
  return L"Unknown";
}


class Module
{
public:
  Module(Prototype* prototype);
  virtual ~Module() {}

  void setStation(Station* station) {mStation = station;}
  Station* getStation() {return mStation;}

  void setPosition(const Vector& position);
  const Vector& getPosition() const;
  const Vector& getWorldPosition() const;
  void setOrientation(const Quaternion& orientation);
  const Quaternion& getOrientation() const;
  void setTransform(const Matrix& transform);
  Matrix getTransform() const;
  bool canRotate() const;

  inline Prototype* getPrototype() {return mPrototype;}  

  bool isOnline() const;
  void setOnline(bool online);
  bool isEnergyOnline() const;
  void setEnergyOnline(bool online);
  bool isThermalOnline() const;
  void setThermalOnline(bool online);

  // ----------------------------------------
  // Diffuse resources

  float getLifeSupport();
  float getLifeSupportBase();
  void setLifeSupportBase(float lifeSupportBase);

  float getStress();
  float getStressBase();
  void setStressBase(float lifeSupportBase);
  
  // ----------------------------------------
  // Generic and flow

  float getDiscreteCurrent(DiscreteResource resource) const;
  float getDiscreteTarget(DiscreteResource resource) const;

  float getFlowLocal(DiscreteResource resource);
  void setFlowLocal(DiscreteResource resource, float value);

  float getFlowTotal() const;
  void updateFlowTotal(float discreteTime, DiscreteResource index, float flow);

  // ----------------------------------------
  // DES control

  DiscreteComponent* getDiscreteComponent(int index) const;
  void preloadDES();

  // ----------------------------------------
  // Sim values

  void setProfitResource(DiscreteResource resource, bool value) {mProfitResource[resource] = value;}
  float getProfit() {return mProfitCurrent;}

  void advanceTurn();

  // ----------------------------------------
  // Port handling

  bool isPortConnected(unsigned int portIndex) {return mConnections[portIndex] != NULL;}
  const ConnectionList& getConnections() {return mConnections;}
  void connectPort(unsigned int iMyPort, Module* otherModule, unsigned int iOtherPort);
  void disconnectPort(unsigned int portIndex);
  void disconnectAllPorts();
  Matrix getFullPortTransform(unsigned int portIndex);
private:
  Prototype* mPrototype;  
  Station* mStation;

  Vector position;
  Quaternion orientation;
  
  bool mEnergyOnline;
  bool mThermalOnline;  

  float mFlowTotal[DISCRETE_NUM];

  DiscreteComponent* mDiscreteComponents[DISCRETE_NUM];

  bool mProfitResource[DISCRETE_NUM];
  float mProfitCurrent;

  vector<Module*> mConnections;
};

#endif
