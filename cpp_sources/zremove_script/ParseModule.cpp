//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#include "Common.hpp"

#include "LuaManager.hpp"
#include "Parser.hpp"
#include "game/Module.hpp"
#include "game/Port.hpp"
#include "widgets/CollisionManager.hpp"

// --------------------------------------------------------------------------------
// Parse info section

void
parseInfo(Module* prototype, LuaObject infoSpec)
{
  CHECK("Module name is string", infoSpec["name"].IsString());
  string name = infoSpec["name"].GetString();
  prototype->setName(name);

  // add authorship information
}


//--------------------------------------------------------------------------------
// Utility functions

Quaternion
parseQuaternion(LuaObject target, string errorPrefix)
{
  CHECK(errorPrefix + ": Quaternion specification is table", target.IsTable());

  CHECK(errorPrefix + ": axis x coordinate is number", target["axis_x"].IsNumber());
  Real xPos = target["axis_x"].GetFloat();
  CHECK(errorPrefix + ": axis y coordinate is number", target["axis_y"].IsNumber());
  Real yPos = target["axis_y"].GetFloat();
  CHECK(errorPrefix + ": axis z coordinate is number", target["axis_z"].IsNumber());
  Real zPos = target["axis_z"].GetFloat();
  CHECK(errorPrefix + ": angle is number", target["angle"].IsNumber());
  Real angle = target["angle"].GetFloat();
  
  Vector3 axis(xPos, yPos, zPos);
  axis.normalise();
  return Quaternion(Degree(angle), axis);
}

Vector3
parseVector3(LuaObject target, string errorPrefix)
{
  CHECK(errorPrefix + ": Vector3 specification is table", target.IsTable());

  CHECK(errorPrefix + ": position x coordinate is number", target["x"].IsNumber());
  Real xPos = target["x"].GetFloat();
  CHECK(errorPrefix + ": position y coordinate is number", target["y"].IsNumber());
  Real yPos = target["y"].GetFloat();
  CHECK(errorPrefix + ": position z coordinate is number", target["z"].IsNumber());
  Real zPos = target["z"].GetFloat();
  
  return Vector3(xPos, yPos, zPos);
}


//--------------------------------------------------------------------------------
// Parsing a single port

Port*
parsePort(Module* prototype, LuaObject portSpec)
{
  CHECK("Port specification is table", portSpec.IsTable());
  CHECK("Port name is string", portSpec["name"].IsString());
  string portName = portSpec["name"].GetString();

  CHECK(portName + ": position is table", portSpec["position"].IsTable());
  LuaObject portPosition = portSpec["position"];

  Vector3 position = parseVector3(portPosition, portName);

  CHECK(portName + ": orientation is table", portSpec["orientation"].IsTable());
  LuaObject orientationList = portSpec["orientation"];
  Quaternion orientation = Quaternion::IDENTITY;
  for(int quaternionNum = 1; quaternionNum <= orientationList.GetCount(); ++quaternionNum) {
    LuaObject quaternionSpec = orientationList[quaternionNum];
    Quaternion rotation = parseQuaternion(quaternionSpec, portName);
    orientation = rotation * orientation;
  }

  
  return prototype->addPort(portName, position, orientation);
}

void
parseForm(Module* prototype, LuaObject formSpec, string identifier)
{
  CHECK("Mesh name is string", formSpec["mesh"].IsString());
  string meshName = formSpec["mesh"].GetString();
  prototype->addMesh(meshName);

  if(formSpec["part_mesh"].IsString()) {
    string partMesh = formSpec["part_mesh"].GetString();
    string partTexture = formSpec["part_texture"].GetString();
    prototype->addPart(partMesh, partTexture);
  }
  
  // Create tangent vectors
  if(MeshManager::getSingletonPtr() != NULL) {
    MeshPtr pMesh = MeshManager::getSingleton().load(meshName,
                                                     RESOURCE_GROUP_MAIN,    
                                                     HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, 
                                                     HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
                                                     true, true); //so we can still read it      
    
    unsigned short src, dest;
    if (!pMesh->suggestTangentVectorBuildParams(src, dest)) {
      pMesh->buildTangentVectors(src, dest);
    }
    CollisionManager::getSingleton().addMesh(pMesh);
  }
  
  if(formSpec["icon"].IsString()) {
    prototype->setIconName(formSpec["icon"].GetString());
  }    
  
  CHECK("Ports are listed", formSpec["ports"].IsTable());
  LuaObject portList = formSpec["ports"];
  for(int portNum = 1; portNum <= portList.GetCount(); ++portNum) {
    LuaObject portSpec = portList[portNum];
    Port* port = parsePort(prototype, portSpec);
    LuaManager::getParseConsumer()->consumePort(identifier + ":" + port->getName(), port);
  }
}


//--------------------------------------------------------------------------------
// Parsing the simulation spec

void
parseSimulation(Module* prototype, LuaObject simSpec)
{
  if(simSpec["mass"].IsNumber())
    prototype->setMass(simSpec["mass"].GetFloat());
  if(simSpec["cost"].IsNumber())
    prototype->setHardwareCost(simSpec["cost"].GetFloat());
  if(simSpec["maintenance"].IsNumber())
    prototype->setMaintenanceExpenses(simSpec["maintenance"].GetFloat());

  if(simSpec["structure"].IsNumber())
    prototype->setStructureMax(simSpec["structure"].GetFloat());

  if(simSpec["energy"].IsNumber())
    prototype->setEnergy(simSpec["energy"].GetFloat());
  if(simSpec["thermal"].IsNumber())
    prototype->setThermal(simSpec["thermal"].GetFloat());

  if(simSpec["life_support"].IsNumber())
    prototype->setLifeSupportBase(simSpec["life_support"].GetFloat());

  if(simSpec["flow_capacity"].IsNumber())
    prototype->setFlowCapacity(simSpec["flow_capacity"].GetFloat());

  if(simSpec["passengers"].IsNumber())
    prototype->setFlowLocal(DISCRETE_PASSENGERS, simSpec["passengers"].GetFloat());
  if(simSpec["visitors"].IsNumber())
    prototype->setFlowLocal(DISCRETE_VISITORS, simSpec["visitors"].GetFloat());
  if(simSpec["crew"].IsNumber())
    prototype->setFlowLocal(DISCRETE_CREW, simSpec["crew"].GetFloat());
  if(simSpec["cargo"].IsNumber())
    prototype->setFlowLocal(DISCRETE_CARGO, simSpec["cargo"].GetFloat());
  if(simSpec["commodities"].IsNumber())
    prototype->setFlowLocal(DISCRETE_COMMODITIES, simSpec["commodities"].GetFloat());
  if(simSpec["service"].IsNumber())
    prototype->setFlowLocal(DISCRETE_SERVICE, simSpec["service"].GetFloat());  
  
  if(simSpec["profit_resource"].IsTable()) {
    LuaObject profitSpec = simSpec["profit_resource"];
    for(int index = 1; index <= profitSpec.GetCount(); ++index) {
      if(profitSpec[index].IsString()) {
        string resKey = profitSpec[index].GetString();
        if(resKey == "passengers")
          prototype->setProfitResource(DISCRETE_PASSENGERS, true);
        else if(resKey == "visitors")
          prototype->setProfitResource(DISCRETE_VISITORS, true);
        else if(resKey == "crew")
          prototype->setProfitResource(DISCRETE_CREW, true);
        else if(resKey == "cargo")
          prototype->setProfitResource(DISCRETE_CARGO, true);
        else if(resKey == "commodities")
          prototype->setProfitResource(DISCRETE_COMMODITIES, true);
        else if(resKey == "service")
          prototype->setProfitResource(DISCRETE_SERVICE, true);
        else if(resKey == "life_support")
          prototype->setProfitResource(DISCRETE_LIFESUPPORT, true);
      }
    }
    
    if(simSpec["profit_max"].IsNumber())
      prototype->setProfitMax(simSpec["profit_max"].GetFloat());
  }
}




//--------------------------------------------------------------------------------
// The main hook point for parsing a whole module

int
Parser::parseModule(LuaState* state)
{
  if(LuaManager::getParseConsumer() == NULL) return 0;
  try {
    LuaStack stack(state);
    LuaObject specification = stack[1];
    
    CHECK("Module identifier is string", specification["identifier"].IsString());
    string identifier = specification["identifier"].GetString();
    Module* prototype = new Module(NULL);
    
    if(specification["form"].IsTable() == true) {
      LuaObject formSpec = specification["form"];
      parseForm(prototype, formSpec, identifier);
    }    

    if(specification["simulation"].IsTable() == true) {
      LuaObject simulationSpec = specification["simulation"];
      parseSimulation(prototype, simulationSpec);
    }    
    else {
      logEngineWarn("Warning: module specification contains no simulation part");
    }    

    LuaManager::getParseConsumer()->consumeModule(identifier, prototype);
  }
  catch(Exception& e) {
    logEngineError(string("Error parsing Module ") + LuaManager::getCurrentFileName() + ": " + e.getFullDescription());
  }
  return 0;
}


//--------------------------------------------------------------------------------
// Unit test method for Parser defined here for completness.


void
TestParser::parse_module()
{  
  ifstream testFile("testdata/TestParser/parse_module.lua");
  string fileData;
  string line;
  while(getline(testFile, line))
    fileData += line + "\n";

  module = NULL;
  LuaManager::getSingleton().doString(fileData);

  CHECK("Valid module handle", module != NULL);
  CHECK_EQUAL("Name is valid", string, "parse_module", module->getName());
  CHECK_EQUAL("Icon name is valid", string, "parse_module.png", module->getIconName());
}


