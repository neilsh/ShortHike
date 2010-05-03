//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import BuildInfo;
private import RenderSystem;
private import GUIManager;
private import Location;
private import SkyLayer;
private import Camera;
private import Rect;
private import Font;
private import Button;
private import Main;
private import Version;
private import Platform;

// --------------------------------------------------------------------------------

private Camera mCamera;

Screen doShellScreen(float timeStep, RenderSystem renderSystem, Location location)
{
  if(mCamera is null)
    mCamera = renderSystem.createCamera();
  
  location.update(timeStep);
  Rect screenRect = new Rect(0, 0, renderSystem.getWidth(), renderSystem.getHeight());
  Matrix yawMatrix = createMatrix(createQuaternion(Vector.UNIT_Y, Math.PI));
  Matrix pitchMatrix = createMatrix(createQuaternion(Vector.UNIT_X, -0.35));
  Matrix rollMatrix = createMatrix(createQuaternion(Vector.UNIT_Z, -1.2));
  Matrix viewMatrix = yawMatrix * pitchMatrix * rollMatrix;
  renderSystem.setView(mCamera.getProjectionMatrix(), viewMatrix);
  doSkyLayer(renderSystem, location, screenRect);
  
  doText(cID(&doShellScreen, 100), BUILD_VERSION, 15, Color.WHITE, renderSystem.getWidth() - 150, renderSystem.getHeight() - 32);
  
  return doButtons(timeStep, renderSystem);
}

// --------------------------------------------------------------------------------
// Main menu


private Screen
doButtons(float timeStep, RenderSystem renderSystem)
{
  if(doMenuButton(cID(&doButtons, 0), "Free Building", 0)) {
    return Screen.STATION;
  }
  if(doMenuButton(cID(&doButtons, 10), "Module Editor", 1)) {
    return Screen.TOOLS;
  }
//   if(doMenuButton(cID(&doButtons, 11), "Location Editor", 5)) {
//     //       return Screen.TOOLS;
//   }

//   version (Standard) {
//   if(doMenuButton(cID(&doButtons, 12), "Buy Pro version", 8)) {
//     rPlatform().openBuyPage();
//   }    
//   }

    if(doMenuButton(cID(&doButtons, 50), "Quit", 3)) {
      rMain().exit();
    }  

  return Screen.SHELL;
}

