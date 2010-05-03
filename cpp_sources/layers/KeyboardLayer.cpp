//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "KeyboardLayer.hpp"

#include "BuildLayer.hpp"
#include "InfoBrowser.hpp"
#include "ProfileLayer.hpp"

#include "game/Mission.hpp"
#include "main/Main.hpp"
#include "main/GamePlay.hpp"


// --------------------------------------------------------------------------------
// Singleton implementation

KeyboardLayer*
rKeyboardLayer()
{
  static KeyboardLayer* mKeyboardLayer = new KeyboardLayer();
  return mKeyboardLayer;
}

// --------------------------------------------------------------------------------

KeyboardLayer::KeyboardLayer()
{
}

bool
KeyboardLayer::keyboardEvent(char keyCode, bool pressed)
{
  if(!pressed) return false;
  
  if(VK_F1 == keyCode) {
    rInfoBrowser()->setVisible(!(rInfoBrowser()->getVisible()));
  }
  else if(VK_F2 == keyCode) {
    rMain()->setMode(Main::MODE_GAMEPLAY);
    return true;
  }
  else if(VK_F3 == keyCode) {
    rMain()->setMode(Main::MODE_MISSION_EDITOR);
    return true;
  }
  else if(VK_F4 == keyCode) {
    rMain()->setMode(Main::MODE_MODULE_EDITOR);
    return true;
  }
  else if(VK_F5 == keyCode) {
    rMain()->setMode(Main::MODE_LOCATION_EDITOR);
    return true;
  }
  else if(VK_F7 == keyCode) {
    rProfileLayer()->setVisible(!(rProfileLayer()->getVisible()));
  }  
  else if(VK_F6 == keyCode) {
    rMain()->setProfiling(!(rMain()->isProfiling()));
  }
  
  else if(keyCode == VK_ESCAPE) {
    //    SoundManager::playSoundEffect("guilib/escape");
    rBuildLayer()->setReadyState();
    return true;
  }
  else if(VK_RETURN == keyCode) {
    rGamePlay()->getMission()->advanceTurn();
  }  
  else if('X' == keyCode) {
    rBuildLayer()->rotateLeft();
    return true;
  }
  else if('C' == keyCode) {
    rBuildLayer()->rotateRight();
    return true;
  }
  else if('V' == keyCode) {
    rBuildLayer()->switchPort();
    return true;
  }
  else if(keyCode == VK_DELETE) {
    rBuildLayer()->deleteModule();
    return true;
  }
  
  return false;
}

