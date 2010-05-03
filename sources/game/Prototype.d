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

// #include "Common.hpp"
// #include "Prototype.hpp"
// #include "Module.hpp"
// #include "Port.hpp"
private import CacheManager;
private import ChunkData;
private import ChunkStream;

private import Mesh;
private import Texture;
private import RenderSystem;

// #include "file/CacheManager.hpp"
// #include "file/ChunkStream.hpp"
// #include "file/ChunkData.hpp"
// #include "main/Platform.hpp"
// #include "main/Timer.hpp"
// #include "rendering/Mesh.hpp"
// #include "rendering/RenderSystem.hpp"
// #include "rendering/Surface.hpp"
// #include "rendering/Texture.hpp"


// You can only add to this list, not take away or change order
enum SimFloat {
  MASS,
  COST_HARDWARE,
  COST_MAINTENANCE,
  STRUCTURE_MAX,
  ENERGY,
  THERMAL,
  PROFIT_MAX,
  FLOW_CAPACITY,
  LIFESUPPORT_BASE,
  FLOW_PASSENGERS,
  FLOW_VISITORS,
  FLOW_CREW,
  FLOW_SERVICE,
  FLOW_CARGO,
  FLOW_COMMODITIES,
}

enum SimBool {
  PROFIT_PASSENGERS,
  PROFIT_VISITORS,
  PROFIT_CREW,
  PROFIT_SERVICE,
  PROFIT_CARGO,
  PROFIT_COMMODITIES,
}

enum SimString {
  NAME,
  DESCRIPTION
}


class Prototype
{
  Port[] getPorts() {return mPorts;}
  Port getPort(int iPort) {if(iPort >= mPorts.length) return null; else return mPorts[iPort];}
  void addPort(Port port) {mPorts ~= port;}

  Mesh getMesh() {touch(); return mMesh;}
  Texture getColorTexture() {touch(); return mColorTexture;}
  Texture getIconTexture()  {return mIconTexture;}
  
  
  float getSimFloat(SimFloat type) {return mSimFloats[type];}
  void setSimFloat(SimFloat type, float value) {mSimFloats[type] = value;}
  bool getSimBool(SimBool type) {return mSimBools[type];}
  void setSimBool(SimBool type, bool value) {mSimBools[type] = value;}
  char[] getSimString(SimString type) { return mSimStrings[type]; }
  void setSimString(SimString type, char[] value) {mSimStrings[type] = value;}  

  // --------------------------------------------------------------------------------

  this(char[] fileName, char[] iconFile, bool fullLoad)
  {
    mChunkData = null;
    mIsLoaded = false;
    mMesh = null;
    mColorTexture = null;

    mMesh = rRenderSystem().createMesh(true);

    for(int iFloat = 0; iFloat <= SimFloat.max; ++iFloat) {
      mSimFloats[iFloat] = 0;
    }  
    for(int iBool = 0; iBool <= SimBool.max; ++iBool) {
      mSimBools[iBool] = false;
    }
  

    mColorTexture = rRenderSystem().createTexture();
    if(fileName != "") {
      load(fileName, fullLoad);
    }
    else {
      clear();
    }

    mIconTexture = rRenderSystem().createTexture();
    mIconTexture.loadFromPNG(iconFile);

    setSimString(SimString.NAME, "Unnamed");
  }

  ~this()
  {
  }


  // --------------------------------------------------------------------------------

  bool
  load(char[] fileName, bool fullLoad)
  {  
    ChunkData chunkData;
    if(!rCacheManager().aquireChunkData(fileName, chunkData)) {
      logError << "Unable to load Prototype";
      return false;
    }

    bool result = load(chunkData, fullLoad);
    if(fullLoad)
      rCacheManager().releaseChunkData(fileName);
    return result;
  }


  // --------------------------------------------------------------------------------


  bool
  load(ChunkData chunkData, bool fullLoad)
  {
    mChunkData = chunkData;
  
    ChunkIterator curr = mChunkData.begin();

    // Ports should not be pointed to ..
    for(int iPort = 0; iPort < mPorts.length; ++iPort) {
      Port port = mPorts[iPort];
      delete port;
    }
    mPorts.length = 0;  
  
    // Handle all chunks we know until MESH
    while(curr != mChunkData.end() && curr.getChunkType() != "MESH") {
      // Port definition
      if(curr.getChunkType() == "PORT") {
        int portNum;
        curr.read(portNum);
        for(int iPort = 0; iPort < portNum; ++iPort) {
          Port port = new Port();
          int portType;
          Vector position;
          Quaternion orientation;
          curr.read(portType);
          curr.readExact(cast(ubyte*)&position, position.sizeof);
          curr.readExact(cast(ubyte*)&orientation, orientation.sizeof);
          port.setPosition(position);
          port.setOrientation(orientation);
          mPorts ~= port;
        }
        curr++;
      }
      // Unknown chunk
      else {
        curr++;
      }
    }

    mIsLoaded = false;
    mMeshIterator = curr.copy;
    if(curr == mChunkData.end() || curr.getChunkType() != "MESH") return false;
    curr++;
    if(curr != mChunkData.end() && curr.getChunkType() == "COLL") curr++;
    if(curr == mChunkData.end() || curr.getChunkType() != "IRGB") return false;
    curr++;

    if(fullLoad) {
      touch();
      mChunkData = null;
    }
  
    return true;
  }

  // --------------------------------------------------------------------------------

  bool
  touch()
  {
    if(mIsLoaded) return true;
    if(mChunkData is null) return false;

    ChunkIterator curr = mMeshIterator;
    if(!mMesh.load(curr, mChunkData.end(), curr)) return false;
    if(!mColorTexture.load(curr, mChunkData.end(), curr)) return false;
  
    mIsLoaded = true;
    return true;
  }


  // --------------------------------------------------------------------------------

  bool
  save(ChunkStream stream)
  {
    // This is a placeholder for potential multiple port types in the future
    const int MAGIC_PORT_TYPE_STANDARD = 1;
  
    stream.openChunk("PORT");
    int portNum = mPorts.length;
    stream.write(portNum);
    for(int iPort = 0; iPort < mPorts.length; ++iPort) {
      Port port = mPorts[iPort];
      Vector position = port.getPosition();
      Quaternion orientation = port.getOrientation();

      int portType = MAGIC_PORT_TYPE_STANDARD;
    
      stream.write(portType);
      stream.writeExact(&position, position.sizeof);
      stream.writeExact(&orientation, orientation.sizeof);
    }
    stream.closeChunk();
    
    mMesh.save(stream);
    mColorTexture.save(stream);

    return true;
  }

  // --------------------------------------------------------------------------------

  void
  clear()
  {
    if(null !is mIconTexture) {
      mIconTexture.loadCheckerBoard();
    }
    if(null !is mMesh) {
      mMesh.loadCube(10);
    }
    if(null !is mColorTexture) {
      mColorTexture.loadCheckerBoard();
    }  

    mPorts.length = 0;

    mIsLoaded = true;
  }



  // --------------------------------------------------------------------------------

  Module
  createModule()
  {
    return new Module(this);
  }


  // --------------------------------------------------------------------------------

  Matrix
  getPortTransform(uint iPort)
  {
    return mPorts[iPort].getTransform();
  }

  Matrix
  getTransformAgainstPort(int myPort, Prototype rhs, int otherPort, float rotation)
  {
    Port connectionPort = getPort(myPort);
    Port targetPort = rhs.getPort(otherPort);
  
    Matrix portToModule = createMatrix(connectionPort.getPosition(), connectionPort.getOrientation());
    Matrix moduleToPort = portToModule.inverse();

    Matrix rotateModule = createMatrix(Vector.ZERO, createQuaternion(Vector.UNIT_Z, radFromDeg(rotation)));
    Matrix flipPort = createMatrix(Vector.ZERO, createQuaternion(Vector.UNIT_X, radFromDeg(180)));
  
    Matrix portToHost = createMatrix(targetPort.getPosition(), targetPort.getOrientation());
    //   Matrix hostToStation(targetPort.getHost().getPosition(), targetPort.getHost().getOrientation());
  
    return moduleToPort * rotateModule * flipPort * portToHost;
   
    //   Matrix moduleToStation = hostToStation * portToHost * flipPort * rotateModule * moduleToPort;
  
    //   oModulePosition = moduleToStation.getPosition();
    //   oModuleOrientation = moduleToStation.getOrientation();
    //   oModuleOrientation.normalize();

  }

  Matrix
  getRotateAroundPort(int myPort, float angle)
  {
    //   int connections = 0;
    //   Port jointPort = null;
    //   for(PortIterator portI = mPorts.begin(); portI != mPorts.end(); ++portI) {
    //     Port currentPort = *portI;
    //     if(currentPort.isConnected()) {
    //       jointPort = currentPort;
    //       connections++;
    //     }
    //   }
    //   if(connections != 1) return;

    Port jointPort = getPort(myPort);  
    Matrix portToUnit = jointPort.getTransform();
    Matrix unitToPort = portToUnit.inverse();
    Matrix rotation = createMatrix(createQuaternion(createVector(0, 0, 1), radFromDeg(angle)));
    
    return unitToPort * rotation * portToUnit;
    //   Matrix newTransform = unitToWorld * portToUnit * rotation * unitToPort;

    //   setTransform(newTransform);

  }

  // --------------------------------------------------------------------------------

  bool
  isIndependentlyOnline()
  {
    return cast(bool)(mSimFloats[SimFloat.ENERGY] >= 0 && mSimFloats[SimFloat.THERMAL] >=0);
  }

private:

  ChunkData mChunkData;
  ChunkIterator mMeshIterator;
  bool mIsLoaded;

  Port[] mPorts;

  Texture mIconTexture;
  Mesh mMesh;
  Texture mColorTexture;

  float mSimFloats[SimFloat.max + 1];
  bool mSimBools[SimBool.max + 1];
  char[] mSimStrings[SimString.max + 1];
}
