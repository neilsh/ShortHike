//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Prototype;
private import Mission;

// guilib
private import Button;
private import GUIManager;
private import Point;
private import Rect;

// rendering
private import RenderSystem;

// --------------------------------------------------------------------------------

Prototype
doModuleDock(WidgetID id, Mission mission)
{
  float timeStep = rGUIManager.timeStep;
  
  const int ICON_MIN_SIZE = 48;
  const int ICON_MAX_SIZE = 128;
  const int ICON_PADDING = 2;
  const float GROWTH_RATE = 40.0;
  const float SHRINK_RATE = 20.0;
  
  const float MAX_WEIGHT = cast(float)ICON_MAX_SIZE / cast(float)ICON_MIN_SIZE;
  const float MIN_WEIGHT = 1.0;
  
  static float weights[];


  Prototype[] prototypes = mission.getPrototypes();
  if(weights.length != prototypes.length) {
    weights.length = prototypes.length;
    weights[] = 1.0;
  }

  // Find rect and indices
  Rect dockRect = new Rect(rRenderSystem.getWidth - ICON_MIN_SIZE, 0, ICON_MIN_SIZE, prototypes.length * ICON_MIN_SIZE);
//   dockRect.x = (rRenderSystem.getWidth() - dockRect.w) / 2;
  Point mousePos = rGUIManager.getMousePosition;
  int mouseIndex = (mousePos.y - dockRect.y) / ICON_MIN_SIZE;
  if(mouseIndex < 0) mouseIndex = 0;
  if(mouseIndex >= prototypes.length) mouseIndex = prototypes.length - 1;

  // Render background box
  doTextureBox(id, "assets/gui/DockBackground.png", dockRect, 8, 8, 8, 8);
  
  // Shrink all icons
  for(int iWeight = 0; iWeight < weights.length; iWeight++) {
    weights[iWeight] -= timeStep * SHRINK_RATE;
  }

  // Hot management
  bool moduleSelected = false;
  if(rGUIManager.isActive(id)) {
    if(rGUIManager.wasKeyReleased(KeyCode.MOUSE1)) {
      if(rGUIManager.isHot(id)) {
        moduleSelected = true;
      }      
      rGUIManager.releaseActive(id);
    }  
  }
  
  if(rGUIManager.isHot(id)) {     
    if(dockRect.contains(mousePos)) {
      weights[mouseIndex] += GROWTH_RATE * timeStep;
    }
    if(rGUIManager.wasKeyPressed(KeyCode.MOUSE1)) {
      rGUIManager.makeActive(id);
    }
  }     
  if(dockRect.contains(mousePos)) {
    rGUIManager.makeHot(id);
  }
  
  // Sum the total weight
  float totalWeight = 0;
  for(int iWeight = 0; iWeight < weights.length; iWeight++) {
    if(weights[iWeight] < MIN_WEIGHT)
      weights[iWeight] = MIN_WEIGHT;
    if(weights[iWeight] > MAX_WEIGHT)
      weights[iWeight] = MAX_WEIGHT;

    totalWeight += weights[iWeight];
  }

  // Render the icons
  Rect srcRect = new Rect(0, 0, 128, 128);
  Rect dstRect = new Rect(dockRect.x, 0, 64, 64);
  float yPos = dockRect.y;
  for(int iProt = 0; iProt < prototypes.length; iProt++) {
    float iconSize = dockRect.h * weights[iProt] / totalWeight;
    Prototype prot = prototypes[iProt];
    dstRect.x = rRenderSystem.getWidth - cast(int)iconSize;
    dstRect.y = cast(int)yPos;
    dstRect.w = cast(int)iconSize;
    dstRect.h = cast(int)iconSize;    
    dstRect.shrink(ICON_PADDING);
    rRenderSystem.renderTexture(prot.getIconTexture, dstRect, srcRect);
    yPos += iconSize;
  }
     
  if(moduleSelected)
    return prototypes[mouseIndex];
  else
    return null;
}
