//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "OrbitState.hpp"
#include "Orbit.hpp"
#include "OrbitSelectedState.hpp"
#include "OrbitAddState.hpp"

#include "audio/SoundManager.hpp"
#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "game/Port.hpp"
#include "game/Station.hpp"
#include "guilib/GUIManager.hpp"
#include "main/Main.hpp"
#include "widgets/DebugTool.hpp"
#include "widgets/CollisionManager.hpp"
#include "widgets/TrackBallCamera.hpp"


//--------------------------------------------------------------------------------

OrbitState::OrbitState(Orbit* _orbitScreen)
  : orbitScreen(_orbitScreen)
{
}

//--------------------------------------------------------------------------------


bool
OrbitState::mouseClickEvent(int buttonID, unsigned clickCount, int x, int y)
{
  if(buttonID == 0 && clickCount == 1) {
    Item* target = pickItem(x, y);
    if(target == NULL) {
      orbitScreen->stateManager.setCurrentState(Orbit::READY_STATE);
    }
    else {
      Module* targetModule = target->castToClass<Module>();
      if(targetModule != NULL) {
        orbitScreen->stateManager.setCurrentState(Orbit::SELECTED_STATE);
        orbitScreen->selectedState->setSelection(targetModule);
      }
    }    
  }
  if(buttonID == 0 && clickCount >= 2) {
    Item* target = pickItem(x, y);
    if(target != NULL) {
      Module* targetModule = target->castToClass<Module>();
      if(targetModule != NULL) {
        orbitScreen->trackBallCamera->setFocus(targetModule->getPosition());
        return true;
      }    
    }  
  }
  return false;
}



bool
OrbitState::mouseDownEvent(int button, int x, int y)
{
  Vector2 mousePositionN = rGUIManager()->getNormalizedCursorPosition(x, y);

  if(button == GUIManager::LeftButton) {
    orbitScreen->trackBallCamera->startOrbit(mousePositionN);
    return true;
  }
  else if(button == GUIManager::RightButton) {
    orbitScreen->trackBallCamera->startZoom(mousePositionN);
    return true;
  }    
  else if(button == GUIManager::MiddleButton) {
    orbitScreen->trackBallCamera->startMove(mousePositionN);
  }
  
  return false;
}

bool
OrbitState::mouseUpEvent(int , int x, int y)
{
  if(TrackBallCamera::ORBITING == orbitScreen->trackBallCamera->getState()) {
    rMain()->getMission()->doString("mission_event_camera_rotate()");
  }
  else if(TrackBallCamera::ZOOMING == orbitScreen->trackBallCamera->getState()) {
    rMain()->getMission()->doString("mission_event_camera_zoom()");
  }  
  orbitScreen->trackBallCamera->stop(rGUIManager()->getNormalizedCursorPosition(x, y));
  return true;
}

void
OrbitState::mouseMotionEvent(int x, int y, float , float, float)
{
  orbitScreen->trackBallCamera->mouseMove(rGUIManager()->getNormalizedCursorPosition(x, y));
}



// //--------------------------------------------------------------------------------
// // Standard key events

bool
OrbitState::keyboardEvent(char keyCode, bool pressed)
{
  if(keyCode == VK_ESCAPE && pressed) {
    SoundManager::playSoundEffect("guilib/escape");
    orbitScreen->stateManager.setCurrentState(Orbit::READY_STATE);
    return true;
  }
  if(pressed && 'M' == keyCode) {
    rMain()->getMission()->creditTransaction(1000);
  }
  if(pressed && 'D' == keyCode && (KEY_MODIFIER_CONTROL & rGUIManager()->getKeyModifier())) {
    DebugTool* debugTool = DebugTool::getSingletonPtr();
    debugTool->setVisible(!debugTool->getVisible());
  }  
  if(pressed && 'R' == keyCode && (KEY_MODIFIER_CONTROL & rGUIManager()->getKeyModifier())) {
    rMain()->getMission()->getStation()->stabilizeSimulation();
  }  
  if(pressed && 'S' == keyCode && (KEY_MODIFIER_CONTROL & rGUIManager()->getKeyModifier())) {
    rMain()->getMission()->doString("ui_show_author_menu()");
  }  
  if(pressed && 'A' == keyCode && (KEY_MODIFIER_CONTROL & rGUIManager()->getKeyModifier())) {
    rMain()->getMission()->doString("ui_show_aspects_menu()");
  }  
  if(pressed && 'Z' == keyCode) {
    orbitScreen->toggleBuildModule();
    return true;
  }
  if(pressed && 'O' == keyCode) {
    SoundManager::getSingleton().toggleSilence();
  }  
  if(pressed && VK_RETURN == keyCode) {
    rMain()->getMission()->advanceTurn();
    return true;
  }
  if(pressed && VK_F5 == keyCode) {
    rMain()->getMission()->reload();
    return true;
  }
  if(keyCode == VK_F8) {
    // FIXME: add new screenshot code
    return true;
  }

  //--------------------------------------------------------------------------------
  // Retrival of quick prototypes
  
  if(pressed && (keyCode == '1' || keyCode == '2' || keyCode == '3' || keyCode == '4' || keyCode == '5')) {
    int quickIndex = keyCode - '1';
    if(orbitScreen->quickPrototypes[quickIndex] == NULL) return true;

    orbitScreen->stateManager.setCurrentState(Orbit::ADD_STATE);
    orbitScreen->addState->setPrototype(orbitScreen->quickPrototypes[quickIndex]);
    SoundManager::playSoundEffect("guilib/button_click");  
    return true;
  }
  
  return false;
}


//--------------------------------------------------------------------------------
// Utility functions

Item*
OrbitState::pickItem(int x, int y)
{
  Vector2 pickPos = rGUIManager()->getRelativeCursorPosition(x, y);
  Ray pickRay = orbitScreen->trackBallCamera->getCamera()->getCameraToViewportRay(pickPos[0], pickPos[1]);
  
  RaySceneQuery* query = orbitScreen->sceneManager->createRayQuery(pickRay, 0xFFFFFFFF);
  query->setSortByDistance(true);
  RaySceneQueryResult& intersections = query->execute();
  Item* pickedItem = NULL;
  float pickedDistance = FLT_MAX;
  for(RaySceneQueryResult::const_iterator objectI = intersections.begin(); objectI != intersections.end(); ++objectI) {
    RaySceneQueryResultEntry entry = *objectI;

    if(entry.distance > pickedDistance) return pickedItem;

    if(entry.movable != NULL) {
      if(entry.movable->isVisible() == false) continue;
      UserDefinedObject* userObject = entry.movable->getUserObject();
      // MovableObjects and Entities aren't dynamic so we can't know if we hit one. But we know UserObjects are only attached
      // to Entities.
      if(userObject != NULL) {
        Entity* entity = static_cast<Entity*>(entry.movable);
        Item* target = dynamic_cast<Item*>(userObject);
        if(target != NULL) {
          Real distance;
          if(CollisionManager::getSingleton().isIntersecting(pickRay, entity, distance) && distance < pickedDistance) {
            pickedItem = target;
            pickedDistance = distance;
          }
        }
      }
    }
  }
  return pickedItem;
}
