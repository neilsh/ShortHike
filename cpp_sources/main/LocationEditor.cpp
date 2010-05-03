//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "LocationEditor.hpp"
#include "file/ChunkData.hpp"
#include "file/ChunkStream.hpp"
#include "game/Location.hpp"
#include "guilib/GUIManager.hpp"
#include "layers/KeyboardLayer.hpp"
#include "layers/CameraLayer.hpp"
#include "layers/ProfileLayer.hpp"
#include "layers/SkyLayer.hpp"
#include "main/Platform.hpp"
#include "rendering/RenderSystem.hpp"

// --------------------------------------------------------------------------------
// Singleton implementation

LocationEditor*
rLocationEditor()
{
  static LocationEditor* mLocationEditor = new LocationEditor();
  return mLocationEditor;
}

// --------------------------------------------------------------------------------

LocationEditor::LocationEditor()
  : mCamera(rRenderSystem()->createCamera()), mLocation(NULL)
{
  mLocation = new Location();

  string fileName = "assets/cislunar/low_earth_orbit.shd";

//   mLocation->load(fileName);

  mLocation->loadDefaultLocation();
  ChunkStream* testLocation = new ChunkStream(fileName);
  mLocation->save(testLocation);
  testLocation->close();
  delete testLocation;
}

// --------------------------------------------------------------------------------

void
LocationEditor::enterState()
{
  rGUIManager()->clearLayers();
  rGUIManager()->addLayer(rSkyLayer());
  rGUIManager()->addLayer(rCameraLayer());
  rGUIManager()->addLayer(rKeyboardLayer());
  rGUIManager()->addLayer(rProfileLayer());
  
  rCameraLayer()->setCamera(mCamera);
  rSkyLayer()->setCamera(mCamera);
  rSkyLayer()->setLocation(mLocation);

}

void
LocationEditor::exitState()
{
  rGUIManager()->clearLayers();
  rCameraLayer()->setCamera(NULL);
  rSkyLayer()->setCamera(NULL);
  rSkyLayer()->setLocation(NULL);
}

