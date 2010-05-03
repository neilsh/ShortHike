//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "GamePlay.hpp"
#include "game/Mission.hpp"
#include "guilib/GUIManager.hpp"
#include "layers/CameraLayer.hpp"
#include "layers/BuildLayer.hpp"
#include "layers/InfoBrowser.hpp"
#include "layers/KeyboardLayer.hpp"
#include "layers/ModuleLayer.hpp"
#include "layers/PrototypeLayer.hpp"
#include "layers/ProfileLayer.hpp"
#include "layers/SkyLayer.hpp"
#include "layers/StationLayer.hpp"
#include "layers/StatusLayer.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/Texture.hpp"

// --------------------------------------------------------------------------------
// Singleton implementation

GamePlay*
rGamePlay()
{
  static GamePlay* mGamePlay = new GamePlay();
  return mGamePlay;
}

// --------------------------------------------------------------------------------

GamePlay::GamePlay()
  : mActive(false), mCamera(rRenderSystem()->createCamera()), mMission(NULL)
{
}

// --------------------------------------------------------------------------------

void
GamePlay::enterState()
{
  rGUIManager()->clearLayers();
  rGUIManager()->addLayer(rSkyLayer());
  rGUIManager()->addLayer(rCameraLayer());
  rGUIManager()->addLayer(rKeyboardLayer());
  rGUIManager()->addLayer(rStationLayer());
  rGUIManager()->addLayer(rBuildLayer());
  rGUIManager()->addLayer(rStatusLayer());
  rGUIManager()->addLayer(rModuleLayer());
  rGUIManager()->addLayer(rInfoBrowser());
  rGUIManager()->addLayer(rProfileLayer());
  
  rSkyLayer()->setCamera(mCamera);
  rStationLayer()->setCamera(mCamera);
  rCameraLayer()->setCamera(mCamera);
  rBuildLayer()->setCamera(mCamera);

  if(mMission != NULL) {
    rSkyLayer()->setLocation(mMission->getLocation());
    rStationLayer()->setStation(mMission->getStation());
    rBuildLayer()->setStation(mMission->getStation());
    rModuleLayer()->setMission(mMission);
  }
  
  mActive = true;
}

void
GamePlay::exitState()
{
  mActive = false;
  
  rGUIManager()->clearLayers();

  rSkyLayer()->setCamera(NULL);
  rSkyLayer()->setLocation(NULL);
  rStationLayer()->setCamera(NULL);
  rStationLayer()->setStation(NULL);
  rCameraLayer()->setCamera(NULL);
  rBuildLayer()->setStation(NULL);
  rBuildLayer()->setCamera(NULL);
  rModuleLayer()->setMission(NULL);
}

// --------------------------------------------------------------------------------

void
GamePlay::update(float)
{
  if(NULL != mMission)
    mMission->updateSimulation();
}


// --------------------------------------------------------------------------------

void
GamePlay::setMission(Mission* mission)
{
  mMission = mission;
  if(mActive && NULL != mMission) {
    rBuildLayer()->setStation(mMission->getStation());
    rStationLayer()->setStation(mMission->getStation());
    rSkyLayer()->setLocation(mMission->getLocation());
    rModuleLayer()->setMission(mMission);
  }  
}
