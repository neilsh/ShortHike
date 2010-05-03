//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "ModuleEditor.hpp"
#include "game/Part.hpp"
#include "guilib/GUIManager.hpp"
#include "layers/KeyboardLayer.hpp"
#include "layers/CameraLayer.hpp"
#include "layers/PrototypeLayer.hpp"
#include "layers/ProfileLayer.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/Texture.hpp"

// --------------------------------------------------------------------------------
// Singleton implementation

ModuleEditor*
rModuleEditor()
{
  static ModuleEditor* mModuleEditor = new ModuleEditor();
  return mModuleEditor;
}

// --------------------------------------------------------------------------------

ModuleEditor::ModuleEditor()
  : mCamera(rRenderSystem()->createCamera())
{
}

// --------------------------------------------------------------------------------

void
ModuleEditor::enterState()
{
  rGUIManager()->clearLayers();
  rGUIManager()->addLayer(rCameraLayer());
  rGUIManager()->addLayer(rKeyboardLayer());
  rGUIManager()->addLayer(rPrototypeLayer());
  rGUIManager()->addLayer(rProfileLayer());
  
  rCameraLayer()->setCamera(mCamera);
}

void
ModuleEditor::exitState()
{
  rGUIManager()->clearLayers();
  rCameraLayer()->setCamera(NULL);
}

