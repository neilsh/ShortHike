//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "Prototype.hpp"
#include "Module.hpp"
#include "Port.hpp"

#include "file/CacheManager.hpp"
#include "file/ChunkStream.hpp"
#include "file/ChunkData.hpp"
#include "main/Platform.hpp"
#include "main/Timer.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/Surface.hpp"
#include "rendering/Texture.hpp"

// --------------------------------------------------------------------------------

Prototype::Prototype(string fileName, bool fullLoad)
  : mChunkData(NULL), mIsLoaded(false), mMesh(NULL), mColorTexture(NULL)
{
  mIconSurface48 = rRenderSystem()->createSurface(48, 48);
  mIconSurface128 = rRenderSystem()->createSurface();
  mMesh = rRenderSystem()->createMesh(true);

  for(int iFloat = 0; iFloat < SIM_FLOAT_NUM; ++iFloat) {
    mSimFloats[iFloat] = 0;
  }  
  for(int iBool = 0; iBool < SIM_BOOL_NUM; ++iBool) {
    mSimBools[iBool] = false;
  }
  

  mColorTexture = rRenderSystem()->createTexture();
  if(fileName != "") {
    load(fileName, fullLoad);
  }
  else {
    clear();
  }
}

Prototype::~Prototype()
{
  if(mChunkData != NULL)
    delete mChunkData;
  if(mMesh != NULL)
    rRenderSystem()->destroyMesh(mMesh);
  if(mColorTexture != NULL)
    rRenderSystem()->destroyTexture(mColorTexture);
}


// --------------------------------------------------------------------------------

bool
Prototype::load(string fileName, bool fullLoad)
{  
  ChunkData* chunkData;
  if(!rCacheManager()->aquireChunkData(fileName, chunkData)) {
    logEngineError("Unable to load Prototype");
    return false;
  }

  bool result = load(chunkData, fullLoad);
  if(fullLoad)
    rCacheManager()->releaseChunkData(fileName);
  return result;
}


// --------------------------------------------------------------------------------

#pragma warning(disable: 4100)

bool
Prototype::load(ChunkData* chunkData, bool fullLoad)
{
  mChunkData = chunkData;
  
  ChunkIterator curr = mChunkData->begin();

  // Ports should not be pointed to ..
  for(PortList::iterator iPort = mPorts.begin(); iPort != mPorts.end(); ++iPort) {
    Port* port = *iPort;
    delete port;
  }
  mPorts.resize(0);  
  
  // Handle all chunks we know until MESH
  while(curr != mChunkData->end() && curr.getChunkType() != "MESH") {
    // Port definition
    if(curr.getChunkType() == "PORT") {
      int portNum;
      curr.read(portNum);
      for(int iPort = 0; iPort < portNum; ++iPort) {
        Port* port = new Port();
        int portType;
        Vector position;
        Quaternion orientation;
        curr.read(portType);
        curr.read(&position, sizeof(Vector));
        curr.read(&orientation, sizeof(Quaternion));
        port->setPosition(position);
        port->setOrientation(orientation);
        mPorts.push_back(port);
      }
      ++curr;
    }
    // Icon definition
    else if(curr.getChunkType() == "IRGB") {
      mIconSurface128->load(curr, mChunkData->end(), curr);
      updateIcons();
    }
    // Unknown chunk
    else {
      ++curr;
    }
  }

  mIsLoaded = false;
  mMeshIterator = curr;
  if(curr == mChunkData->end() || curr.getChunkType() != "MESH") return false;
  ++curr;
  if(curr != mChunkData->end() && curr.getChunkType() == "COLL") ++curr;
  if(curr == mChunkData->end() || curr.getChunkType() != "IRGB") return false;
  ++curr;

  if(fullLoad) {
    touch();
    mChunkData = NULL;
  }
  
  return true;
}

// --------------------------------------------------------------------------------

bool
Prototype::touch()
{
  if(mIsLoaded) return true;
  if(NULL == mChunkData) return false;

  ChunkIterator curr = mMeshIterator;
  if(!mMesh->load(curr, mChunkData->end(), curr)) return false;
  if(!mColorTexture->load(curr, mChunkData->end(), curr)) return false;
  
  mIsLoaded = true;
  return true;
}


// --------------------------------------------------------------------------------

bool
Prototype::save(ChunkStream* stream)
{
  // This is a placeholder for potential multiple port types in the future
  const MAGIC_PORT_TYPE_STANDARD = 1;
  
  stream->openChunk("PORT");
  int portNum = mPorts.size();
  stream->write(portNum);
  for(PortList::iterator iPort = mPorts.begin(); iPort != mPorts.end(); ++iPort) {
    Port* port = *iPort;
    const Vector& position = port->getPosition();
    const Quaternion& orientation = port->getOrientation();

    int portType = MAGIC_PORT_TYPE_STANDARD;
    
    stream->write(portType);
    stream->write(&position, sizeof(Vector));
    stream->write(&orientation, sizeof(Quaternion));
  }
  stream->closeChunk();
    
  mIconSurface128->save(stream);
  mMesh->save(stream);
  mColorTexture->save(stream);


  return true;
}

// --------------------------------------------------------------------------------

void
Prototype::clear()
{
  if(NULL != mIconSurface128) {
    mIconSurface128->loadCheckerBoard();
    updateIcons();
  }  
  if(NULL != mMesh) {
    mMesh->loadCube(10);
  }
  if(NULL != mColorTexture) {
    mColorTexture->loadCheckerBoard();
  }  

  for(PortList::iterator iPort = mPorts.begin(); iPort != mPorts.end(); ++iPort) {
    Port* port = *iPort;
    delete port;
  }
  mPorts.resize(0);  

  mIsLoaded = true;
}

// --------------------------------------------------------------------------------

void
Prototype::updateIcons()
{
  // create the 48x48 icon from the 
  int* src = static_cast<int*>(mIconSurface128->getData());
  int* dest = static_cast<int*>(mIconSurface48->getData());
  for(int destY = 0; destY < mIconSurface48->getHeight(); ++destY) {
    for(int destX = 0; destX < mIconSurface48->getWidth(); ++destX) {
      int srcX = destX * mIconSurface128->getWidth() / mIconSurface48->getWidth();
      int srcY = destY * mIconSurface128->getHeight() / mIconSurface48->getHeight();
      dest[destY * mIconSurface48->getWidth() + destX] = src[srcY * mIconSurface128->getWidth() + srcX];
    }
  }
  mIconSurface48->flush();
}



// --------------------------------------------------------------------------------

Module*
Prototype::createModule()
{
  return new Module(this);
}


// --------------------------------------------------------------------------------

Matrix
Prototype::getPortTransform(unsigned int iPort)
{
  return mPorts[iPort]->getTransform();
}

Matrix
Prototype::getTransformAgainstPort(int myPort, Prototype* rhs, int otherPort, float rotation)
{
  Port* connectionPort = getPort(myPort);
  Port* targetPort = rhs->getPort(otherPort);
  
  Matrix portToModule(connectionPort->getPosition(), connectionPort->getOrientation());
  Matrix moduleToPort = portToModule.inverse();

  Matrix rotateModule(Vector::ZERO, Quaternion(Vector::UNIT_Z, rotation));
  Matrix flipPort(Vector::ZERO, Quaternion(Vector::UNIT_X, Degree(180)));
  
   Matrix portToHost(targetPort->getPosition(), targetPort->getOrientation());
//   Matrix hostToStation(targetPort->getHost()->getPosition(), targetPort->getHost()->getOrientation());
  
   return moduleToPort * rotateModule * flipPort * portToHost;
   
//   Matrix moduleToStation = hostToStation * portToHost * flipPort * rotateModule * moduleToPort;
  
//   oModulePosition = moduleToStation.getPosition();
//   oModuleOrientation = moduleToStation.getOrientation();
//   oModuleOrientation.normalize();

}

Matrix
Prototype::getRotateAroundPort(int myPort, float angle)
{
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

  Port* jointPort = getPort(myPort);  
  Matrix portToUnit = jointPort->getTransform();
  Matrix unitToPort = portToUnit.inverse();
  Matrix rotation(Quaternion(Vector(0, 0, 1), angle));
    
  return unitToPort * rotation * portToUnit;
//   Matrix newTransform = unitToWorld * portToUnit * rotation * unitToPort;

//   setTransform(newTransform);

}

// --------------------------------------------------------------------------------

bool
Prototype::isIndependentlyOnline()
{
  return mSimFloats[SIM_ENERGY] >= 0 && mSimFloats[SIM_THERMAL] >=0;
}
