//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "SkyLayer.hpp"

#include "game/Location.hpp"
#include "rendering/Camera.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/Texture.hpp"
//#include "audio/SoundManager.hpp"
// #include "game/Aspects.hpp"
// #include "game/Mission.hpp"
// #include "game/Module.hpp"
// #include "game/Port.hpp"
// #include "game/Station.hpp"
// #include "guilib/GUIManager.hpp"
// #include "rendering/Camera.hpp"


// --------------------------------------------------------------------------------
// Singleton implementation

SkyLayer*
rSkyLayer()
{
  static SkyLayer* mSkyLayer = new SkyLayer();
  return mSkyLayer;
}


// --------------------------------------------------------------------------------

SkyLayer::SkyLayer()
  : mLocation(NULL)
{
}

// --------------------------------------------------------------------------------

void
SkyLayer::update(float timeStep)
{
  if(mLocation != NULL)
    mLocation->update(timeStep);
}


// --------------------------------------------------------------------------------

struct PatchInstance
{
  Mesh* mesh;
  Texture* groundTexture;
  Texture* cloudTexture;
  float distance;
};

const float PLANET_ANGLE = 0.8f;

void
SkyLayer::render(RenderSystem* renderSystem, int, int)
{
  if(mLocation == NULL || mCamera == NULL) return;
  
  renderSystem->enableColorTexture(true);
  renderSystem->enableAlphaBlending(true);
  renderSystem->enableLighting(false);
  renderSystem->enableZBuffer(false);
  
  Matrix viewMatrix = mCamera->getViewMatrix();
  viewMatrix.setTranslation(Vector::ZERO);
  renderSystem->setProjectionMatrix(mCamera);
  renderSystem->setViewMatrix(viewMatrix);
  
  // ----------------------------------------
  // Render Sky Objects

  const SkyObjectList& skyObjects = mLocation->getSkyObjects();
  for(SkyObjectList::const_iterator iObject = skyObjects.begin(); iObject != skyObjects.end(); ++iObject) {
    const SkyObject& object = *iObject;
    renderSystem->setColorTexture(object.texture);
    renderSystem->setMesh(object.mesh);
    renderSystem->renderMesh(Matrix::IDENTITY);
  }  

  // ----------------------------------------
  // Render Planet

  vector<PlanetPatch> visiblePatches;
  const PlanetPatchList& patches = mLocation->getPlanetPatches();

  // Culling here
  Matrix planetRotate = mLocation->getPlanetRotate();
  for(unsigned int iPatch = 0; iPatch < patches.size(); ++iPatch) {
    const PlanetPatch& patch = patches[iPatch];
    Vector position = patch.groundMesh->getMeshCenter();
    position.normalize();
    position = position * planetRotate;
    if(acos(position.dotProduct(Vector::UNIT_Y)) < PLANET_ANGLE) {
      // Visible, force loading and add to renderable
      mLocation->touchPatch(iPatch);
      visiblePatches.push_back(patch);
    }
  }
  // Sort patches here
  // zzz

  // Render ground
  Matrix planetTransform = mLocation->getPlanetTransform();  
  for(vector<PlanetPatch>::iterator iPatch = visiblePatches.begin(); iPatch != visiblePatches.end(); ++iPatch) {
    PlanetPatch& patch = *iPatch;
    renderSystem->setColorTexture(patch.groundTexture);
    renderSystem->setMesh(patch.groundMesh);
    renderSystem->renderMesh(planetTransform);
  }
  
  // Render ground haze
  if(mLocation->getGroundHazeTexture() != NULL && mLocation->getGroundHazeMesh() != NULL) {
    renderSystem->setColorTexture(mLocation->getGroundHazeTexture());
    renderSystem->setMesh(mLocation->getGroundHazeMesh(), false, false);
    renderSystem->renderMesh(Matrix::IDENTITY);
  }
  
  // Render clouds
  for(vector<PlanetPatch>::iterator iPatch = visiblePatches.begin(); iPatch != visiblePatches.end(); ++iPatch) {
    PlanetPatch& patch = *iPatch;
    renderSystem->setColorTexture(patch.cloudTexture);
    renderSystem->setMesh(patch.cloudMesh);
    renderSystem->renderMesh(planetTransform);
  }
  
  // Render cloud haze
//   if(mLocation->getCloudHazeTexture() != NULL && mLocation->getCloudHazeMesh() != NULL) {
//     renderSystem->setColorTexture(mLocation->getCloudHazeTexture());
//     renderSystem->setMesh(mLocation->getCloudHazeMesh());
//     renderSystem->renderMesh(Matrix::IDENTITY);
//   }


  renderSystem->enableAlphaBlending(false);
  renderSystem->enableLighting(true);
  renderSystem->enableZBuffer(true);
}




