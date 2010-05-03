//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Camera;
private import Mesh;
private import RenderSystem;
private import GUIManager;
private import Point;
private import Rect;

// --------------------------------------------------------------------------------

void
doCameraControl(float timeStep, RenderSystem renderSystem, WidgetID id, Camera camera, Rect clip)
{
  static const float CAMERA_MOVE_SPEED = 25;

  Vector translation = createVector(0, 0, 0);
  if(rGUIManager().isKeyDown(KeyCode.A) || rGUIManager().isKeyDown(KeyCode.LEFT)) translation.x -= 1;
  if(rGUIManager().isKeyDown(KeyCode.D) || rGUIManager().isKeyDown(KeyCode.RIGHT)) translation.x += 1;
  if(rGUIManager().isKeyDown(KeyCode.W) || rGUIManager().isKeyDown(KeyCode.UP)) translation.y += 1;
  if(rGUIManager().isKeyDown(KeyCode.S) || rGUIManager().isKeyDown(KeyCode.DOWN)) translation.y -= 1;
  if(rGUIManager().isKeyDown(KeyCode.R)) translation.z += 1;
  if(rGUIManager().isKeyDown(KeyCode.F)) translation.z -= 1;
  translation *= CAMERA_MOVE_SPEED * timeStep;
  camera.translateLocal(translation);

  if(rGUIManager.isActive(id)) {
    Point pos = rGUIManager().getMousePosition();
    camera.mouseMove(createVector(pos.x, pos.y, 0));
    if(rGUIManager.wasKeyReleased(KeyCode.MOUSE1) || rGUIManager.wasKeyReleased(KeyCode.MOUSE2)) {
      rGUIManager.releaseActive(id);
      camera.stop();
    }
  }
  if(rGUIManager.isHot(id)) {  
    if((rGUIManager.wasKeyPressed(KeyCode.MOUSE1) || rGUIManager.wasKeyPressed(KeyCode.MOUSE2)) &&
       rGUIManager.makeActive(id)) {
      Point pos = rGUIManager().getMousePosition();
      if(rGUIManager.wasKeyPressed(KeyCode.MOUSE1)){
        camera.startOrbit(createVector(pos.x, pos.y, 0));
      }
      else if(rGUIManager.wasKeyPressed(KeyCode.MOUSE2)) {
        camera.startZoom(createVector(pos.x, pos.y, 0));
      }
    }
  }
  if(clip.contains(rGUIManager().getCursorPosition())) {
    rGUIManager().makeHot(id);
  }
}

