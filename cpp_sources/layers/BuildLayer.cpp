//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "BuildLayer.hpp"

//#include "audio/SoundManager.hpp"
#include "game/Aspects.hpp"
#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "game/Port.hpp"
#include "game/Prototype.hpp"
#include "game/Station.hpp"
#include "guilib/GUIManager.hpp"
#include "rendering/Camera.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/Texture.hpp"


// --------------------------------------------------------------------------------
// Singleton implementation

BuildLayer*
rBuildLayer()
{
  static BuildLayer* mBuildLayer = new BuildLayer();
  return mBuildLayer;
}


// --------------------------------------------------------------------------------

BuildLayer::BuildLayer()
  : mState(STATE_READY),
    mStation(NULL), mCamera(NULL), mPrototype(NULL),
    mPortMesh(NULL), mPortTexture(NULL),
    mFocusMesh(NULL), mFocusTexture(NULL),
    mConnectionMesh(NULL), mConnectionTexture(NULL),
    mMouseOver(MOUSEOVER_BACKGROUND), mMouseOverModule(NULL), mMouseOverPortIndex(-1)
{
  RenderSystem* rSystem = rRenderSystem();

  mPortMesh = rSystem->createMesh(true);
  mPortMesh->load("assets/user_interface/port.shd");
  mPortTexture = rSystem->createTexture();
  mPortTexture->loadSolid(0, 0, 0xcf, 0xff);

  mSelectedMesh = rSystem->createMesh();
  mSelectedMesh->loadCube(10);
  mSelectedTexture = rSystem->createTexture();
  mSelectedTexture->loadSolid(0, 0, 0xcf, 0x5f);

  mFocusMesh = rSystem->createMesh();
  mFocusMesh->load("assets/user_interface/port.shd");
  mFocusTexture = rSystem->createTexture();
  mFocusTexture->loadSolid(0, 0, 0xff, 0xb0);

  mConnectionMesh = rSystem->createMesh();
  mConnectionMesh->load("assets/user_interface/connection_marker.shd");
  mConnectionTexture = rSystem->createTexture();
  mConnectionTexture->loadSolid(0, 0, 0xcf, 0xff);

  setReadyState();
}

// --------------------------------------------------------------------------------

void
BuildLayer::setStation(Station* station)
{
  mStation = station;
}


// --------------------------------------------------------------------------------

class ModuleHit
{
public:
  ModuleHit(Module* m, float d)
    : module(m), distance(d) {}

  Module* module;
  float distance;
};

class PortHit
{
public:
  PortHit(Module* m, unsigned int i, float d)
    : portModule(m), portIndex(i), distance(d) {}

  Module* portModule;
  unsigned int portIndex;
  float distance;
};

template<class C>
bool
depthSort(const C& lhs, const C& rhs)
{
  return lhs.distance < rhs.distance;
}


void
BuildLayer::update(float)
{
  if(NULL == mStation || NULL == mCamera) return;
  Prof(collision);
  
  Vector pickPos = rGUIManager()->getRelativeCursorPosition(rGUIManager()->getCursorX(), rGUIManager()->getCursorY());
  Ray pickRay = mCamera->getCameraToViewportRay(pickPos[0], pickPos[1]);
   
  // Only test meshes and ports that have spherical intersections
  vector<ModuleHit> moduleHits;
  vector<PortHit> portHits;
  float oDistanceSphere = FLT_MAX;
  
  {
    const set<Module*>& modules = mStation->getModules();

    for(set<Module*>::const_iterator iModule = modules.begin(); iModule != modules.end(); ++iModule) {
      Module* currentModule = *iModule;
      Matrix moduleTransform(currentModule->getPosition(), currentModule->getOrientation());
      Mesh* moduleMesh = currentModule->getPrototype()->getMesh();
      if(moduleMesh->intersectRayBox(pickRay, moduleTransform, oDistanceSphere)) {
        moduleHits.push_back(ModuleHit(currentModule, oDistanceSphere));
      }
      
      if(STATE_SELECTED == mState && mSelectedModule == currentModule) continue;
      
      if(STATE_ADD == mState || STATE_SELECTED == mState) {
        const PortList& ports = currentModule->getPrototype()->getPorts();
        for(unsigned int iPort = 0; iPort < ports.size(); ++iPort) {
          if(!currentModule->isPortConnected(iPort)) {
            Matrix portTransform(ports[iPort]->getPosition(), ports[iPort]->getOrientation());
            Matrix combinedTransform = portTransform * moduleTransform;
            if(mPortMesh->intersectRayBox(pickRay, combinedTransform, oDistanceSphere)) {
              portHits.push_back(PortHit(currentModule, iPort, oDistanceSphere));
            }
          }
        }
      }
    }
  }
  
  {
    sort(moduleHits.begin(), moduleHits.end(), depthSort<ModuleHit>);
    sort(portHits.begin(), portHits.end(), depthSort<PortHit>);
  }
  
  {
    float minDistance = FLT_MAX;
    mMouseOver = MOUSEOVER_BACKGROUND;
    float oDistanceTri = FLT_MAX;  

    vector<ModuleHit>::iterator iModuleHit = moduleHits.begin();
    vector<PortHit>::iterator iPortHit = portHits.begin();

    while(iModuleHit != moduleHits.end() || iPortHit != portHits.end()) {
      if(iModuleHit == moduleHits.end() || (iPortHit != portHits.end() && (*iPortHit).distance < (*iModuleHit).distance)) {
        PortHit& portHit = *iPortHit;
        if(mPortMesh->intersectRayTri(pickRay, portHit.portModule->getFullPortTransform(portHit.portIndex), oDistanceTri)
           && oDistanceTri < minDistance) {
          mMouseOver = MOUSEOVER_PORT;
          mMouseOverModule = portHit.portModule;
          mMouseOverPortIndex = portHit.portIndex;
          minDistance = oDistanceTri;
        }
        iPortHit++;
      }
      else {
        ModuleHit& moduleHit = *iModuleHit;
        Mesh* moduleMesh = moduleHit.module->getPrototype()->getMesh();
        if(moduleMesh->intersectRayTri(pickRay, moduleHit.module->getTransform(), oDistanceTri) && oDistanceTri < minDistance) {
          mMouseOver = MOUSEOVER_MODULE;
          mMouseOverModule = moduleHit.module;
          minDistance = oDistanceTri;
        }
        iModuleHit++;
      }      

      // abort when the current hit is closer than all remaining bounding spheres
      if(iModuleHit != moduleHits.end() && iPortHit != portHits.end()) {
        if((*iModuleHit).distance > minDistance && (*iPortHit).distance > minDistance) break;
      }
      else if(iModuleHit != moduleHits.end()) {
        if((*iModuleHit).distance > minDistance) break;
      }
      else if(iPortHit != portHits.end()){
        if((*iPortHit).distance > minDistance) break;
      }
    }
  }
}


// --------------------------------------------------------------------------------

void
BuildLayer::render(RenderSystem* renderSystem, int, int)
{
  if(NULL == mStation) return;

  renderSystem->enableColorTexture(true);
  renderSystem->setColorTexture(mPortTexture);
  renderSystem->setMesh(mPortMesh);
  
  if(STATE_ADD == mState || STATE_SELECTED == mState) {
    const set<Module*>& modules = mStation->getModules();
    for(set<Module*>::const_iterator iModule = modules.begin(); iModule != modules.end(); ++iModule) {
      Module* currentModule = *iModule;
      if(STATE_SELECTED == mState && mSelectedModule == currentModule) continue;
      Matrix moduleTransform = currentModule->getTransform();
      Prototype* prototype = currentModule->getPrototype();
      
      const PortList& ports = prototype->getPorts();
      for(unsigned int iPort = 0; iPort < ports.size(); ++iPort) {
        if(!currentModule->isPortConnected(iPort)) {
          Matrix portTransform(ports[iPort]->getPosition(), ports[iPort]->getOrientation());
          Matrix combinedTransform = portTransform * moduleTransform;
          {
            renderSystem->renderMesh(combinedTransform);
          }          
        }
      }
    }
    if(MOUSEOVER_PORT == mMouseOver && mPrototype != NULL) {
      renderPrototype(renderSystem);
    }
  }

  if(STATE_SELECTED == mState && NULL != mSelectedModule) {
    renderSystem->enableAlphaBlending(true);  
    renderSystem->setColorTexture(mSelectedTexture);
    renderSystem->setMesh(mSelectedMesh);
    renderSystem->renderMesh(mSelectedModule->getTransform());
    renderSystem->enableAlphaBlending(false);  
  }  
}

// --------------------------------------------------------------------------------

void
BuildLayer::renderPrototype(RenderSystem* renderSystem)
{
  if(STATE_SELECTED == mState && mMouseOverModule == mSelectedModule) return;
  
  renderSystem->setColorTexture(mPrototype->getColorTexture());
  renderSystem->setMesh(mPrototype->getMesh(), true);
  Matrix portAlignTransform = mPrototype->getTransformAgainstPort(mPort, mMouseOverModule->getPrototype(), mMouseOverPortIndex, mAngle);
  Matrix fullTransform = portAlignTransform * mMouseOverModule->getTransform();
  renderSystem->renderMesh(fullTransform);

  PortConnectionList connected;
  if(STATE_SELECTED == mState && NULL != mSelectedModule)
    connected = mStation->findConnectedPorts(mPrototype, fullTransform, mSelectedModule);
  else
    connected = mStation->findConnectedPorts(mPrototype, fullTransform);

  renderSystem->setColorTexture(mConnectionTexture);
  renderSystem->setMesh(mConnectionMesh, false, false);
  for(unsigned int iPort = 0; iPort < connected.size(); ++iPort) {
    if(NULL != connected[iPort].first) {
      renderSystem->renderMesh(mPrototype->getPort(iPort)->getTransform() * fullTransform);
    }
  }
}



// --------------------------------------------------------------------------------

bool
BuildLayer::mouseClickEvent(int buttonID, unsigned clickCount, int x, int y)
{
  if(0 == buttonID && 1 == clickCount && NULL != mStation) {
    if(STATE_ADD == mState) {
      if(MOUSEOVER_PORT == mMouseOver && NULL != mPrototype) {
        Module* target = mPrototype->createModule();
        Matrix portAlignTransform = mPrototype->getTransformAgainstPort(mPort, mMouseOverModule->getPrototype(), mMouseOverPortIndex, mAngle);
        Matrix fullTransform = portAlignTransform * mMouseOverModule->getTransform();
        target->setTransform(fullTransform);
        mStation->addModule(target);
        Mission* mission = mStation->getMission();
        if(mission->getAspects()->isCredits()) {
          mission->creditTransaction(-mPrototype->getSimFloat(SIM_COST_HARDWARE));
        }
        mission->useActionPoint();        
        // --------------------------------------------------------------------------------
        // Profile: Update number of modules added
        //         ConfigManager::getPlayer()->mModulesAdded++;
        return true;    

      }
    }
    else if(STATE_SELECTED == mState && MOUSEOVER_PORT == mMouseOver && NULL != mSelectedModule) {
        Matrix portAlignTransform = mPrototype->getTransformAgainstPort(mPort, mMouseOverModule->getPrototype(), mMouseOverPortIndex, mAngle);
        Matrix fullTransform = portAlignTransform * mMouseOverModule->getTransform();
        mStation->disconnectModule(mSelectedModule);
        mSelectedModule->setTransform(fullTransform);
        mStation->connectModule(mSelectedModule);
        mStation->getMission()->useActionPoint();
        return true;      
    }
    if(MOUSEOVER_BACKGROUND == mMouseOver) {
      setReadyState();
    }
    else if(MOUSEOVER_MODULE == mMouseOver) {
      setSelectedState(mMouseOverModule);
    }
  }
  
  return false;
}

// --------------------------------------------------------------------------------

void
BuildLayer::setReadyState()
{
  mState = STATE_READY;
}

// --------------------------------------------------------------------------------

void
BuildLayer::setAddState(Prototype* prototype)
{
  mState = STATE_ADD;
  mAngle = 0;
  mPort = 0;
  mPrototype = prototype;
}

// --------------------------------------------------------------------------------

void
BuildLayer::setSelectedState(Module* module)
{
  mSelectedModule = module;
  mAngle = 0;
  mPort = 0;
  if(NULL != mSelectedModule) {
    mState = STATE_SELECTED;
    mPrototype = mSelectedModule->getPrototype();
  }
  else {
    setReadyState();
  }
}

// --------------------------------------------------------------------------------

void
BuildLayer::rotateLeft()
{
  mAngle += Math::PI / 8.0f;
}

void
BuildLayer::rotateRight()
{
  mAngle -= Math::PI / 8.0f;
}

void
BuildLayer::switchPort()
{
  if(NULL != mPrototype) {
    mPort++;
    mPort = mPort % mPrototype->getPorts().size();
  }
}

void
BuildLayer::deleteModule()
{
  if(NULL == mStation || mStation->getModuleCount() == 1) return;

  // Find the potential next module to select
  Module* nextSelection = NULL;
  ConnectionList connections = mSelectedModule->getConnections();
  for(unsigned int iConnection = 0; iConnection < connections.size(); ++iConnection) {
    if(NULL != connections[iConnection]) {
      nextSelection = connections[iConnection];
      break;
    }
  }
     
  mStation->getMission()->useActionPoint();
// // Profile: Update number of modules removed
// ConfigManager::getPlayer()->mModulesDeleted++;

  mStation->removeModule(mSelectedModule);

  if(nextSelection == NULL) {
    setReadyState();
  }
  else {
    setSelectedState(nextSelection);
  }
    
  //SoundManager::playSoundEffect("orbit/undocking");
}


// --------------------------------------------------------------------------------

wstring
BuildLayer::outputString()
{
  if(STATE_READY == mState)
    return L"STATE_READY";
  else if(STATE_ADD == mState)
    return L"STATE_ADD";
  else if(STATE_SELECTED == mState)
    return L"STATE_SELECTED";
  else
    return L"UNKNOWN STATE";
}


// --------------------------------------------------------------------------------

Module*
BuildLayer::getSelectedModule()
{
  if(STATE_SELECTED != mState)
    return NULL;
  else
    return mSelectedModule;
}

