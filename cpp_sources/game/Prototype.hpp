//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef GAME_PROTOTYPE_HPP
#define GAME_PROTOTYPE_HPP

#include "file/Persistent.hpp"
#include "file/ChunkData.hpp"

class ChunkIterator;
class ChunkStream;
class ChunkData;
class Mesh;
class Texture;
class Surface;
class Module;
class Port;

typedef vector<Port*> PortList;


// You can only add to this list, not take away or change order
enum SimFloat {
  SIM_MASS,
  SIM_COST_HARDWARE,
  SIM_COST_MAINTENANCE,
  SIM_STRUCTURE_MAX,
  SIM_ENERGY,
  SIM_THERMAL,
  SIM_PROFIT_MAX,
  SIM_FLOW_CAPACITY,
  SIM_LIFESUPPORT_BASE,
  SIM_FLOW_PASSENGERS,
  SIM_FLOW_VISITORS,
  SIM_FLOW_CREW,
  SIM_FLOW_SERVICE,
  SIM_FLOW_CARGO,
  SIM_FLOW_COMMODITIES,
  SIM_FLOAT_NUM
};

enum SimBool {
  SIM_PROFIT_PASSENGERS,
  SIM_PROFIT_VISITORS,
  SIM_PROFIT_CREW,
  SIM_PROFIT_SERVICE,
  SIM_PROFIT_CARGO,
  SIM_PROFIT_COMMODITIES,
  SIM_BOOL_NUM
};

class Prototype
{
public:
  Prototype(string fileName = "", bool fullLoad = false);

  bool load(string fileName, bool fullLoad = false);
  virtual bool load(ChunkData* chunkData, bool fullLoad = false);
  virtual bool save(ChunkStream* stream);

  void clear();
  
  PortList& getPorts() {return mPorts;}
  Port* getPort(unsigned int iPort) {if(iPort >= mPorts.size()) return NULL; else return mPorts[iPort];}
  void addPort(Port* port) {mPorts.push_back(port);}
  Matrix getPortTransform(unsigned int iPort);

  Surface* getIconSurface128() {return mIconSurface128;}
  Surface* getIconSurface48() {return mIconSurface48;}

  bool touch();
  Mesh* getMesh() {touch(); return mMesh;}
  Texture* getColorTexture() {touch(); return mColorTexture;}
  void updateIcons();

  Matrix getTransformAgainstPort(int myPort, Prototype* rhs, int otherPort, float angle = 0.0f);
  Matrix getRotateAroundPort(int myPort, float angle);

  Module* createModule();

  inline float getSimFloat(SimFloat type) {return mSimFloats[type];}
  inline void setSimFloat(SimFloat type, float value) {mSimFloats[type] = value;}
  inline bool getSimBool(SimBool type) {return mSimBools[type];}
  inline void setSimBool(SimBool type, bool value) {mSimBools[type] = value;}

  bool isIndependentlyOnline();
private:

  ChunkData* mChunkData;
  ChunkIterator mMeshIterator;
  bool mIsLoaded;

  PortList mPorts;

  Surface* mIconSurface128;
  Surface* mIconSurface48;
  Mesh* mMesh;
  Texture* mColorTexture;

  float mSimFloats[SIM_FLOAT_NUM];
  bool mSimBools[SIM_BOOL_NUM];
};

#endif
