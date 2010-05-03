//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "StationLayer.hpp"

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

StationLayer*
rStationLayer()
{
  static StationLayer* mStationLayer = new StationLayer();
  return mStationLayer;
}


// --------------------------------------------------------------------------------

StationLayer::StationLayer()
  : mStation(NULL), mCamera(NULL)
{
}

// --------------------------------------------------------------------------------

void
StationLayer::render(RenderSystem* renderSystem, int, int)
{
  if(NULL == mStation || NULL == mCamera) return;

  renderSystem->setProjectionMatrix(mCamera);
  renderSystem->setViewMatrix(mCamera->getViewMatrix());
  
  renderSystem->enableColorTexture(true);
  renderSystem->enableAlphaBlending(false);

  const set<Module*>& modules = mStation->getModules();

  set<Prototype*> visiblePrototypes;
  vector<Module*> visibleModules;
  
  // Prototype and module visibility check
  for(set<Module*>::const_iterator iModule = modules.begin(); iModule != modules.end(); ++iModule) {
    Module* currentModule = *iModule;
    // CHECK for module visibility here ..
    visiblePrototypes.insert(currentModule->getPrototype());
    visibleModules.push_back(currentModule);
  }
  
  // Batched module rendering
  for(set<Prototype*>::iterator iPrototype = visiblePrototypes.begin(); iPrototype != visiblePrototypes.end(); ++iPrototype) {
    Prototype* prototype = *iPrototype;
    renderSystem->setColorTexture(prototype->getColorTexture());
    renderSystem->setMesh(prototype->getMesh());
    for(vector<Module*>::iterator iModule = visibleModules.begin(); iModule != visibleModules.end(); ++iModule) {
      Module* currentModule = *iModule;
      if(currentModule->getPrototype() == prototype)
        renderSystem->renderMesh(currentModule->getTransform());
    }
  }
}



