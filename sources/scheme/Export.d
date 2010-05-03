// --------------------------------------------------------------------------------
//
// ShortHike, www.shorthike.com
//
// Copyright 2002-2006 Kai Backman, Mistaril Ltd.
//
// --------------------------------------------------------------------------------

private import Common;
private import Value;
private import Eval;
private import Read;
private import Library;

// --------------------------------------------------------------------------------
// We export functions from these

private import Button;
private import GUIManager;
private import Main;
private import Rect;
private import StationScreen;

// --------------------------------------------------------------------------------

Environment
standardEnvironment()
{
  Environment env = libraryEnvironment();
  bindFunction(env, "add-widget", &sAddWidget);

  // GUIManager
  bindFunction(env, "gui-make-active", &eGUIMakeActive);

  // Rect
  bindFunction(env, "rect", &eRect);
  bindFunction(env, "rect-x", &eRectX);
  bindFunction(env, "rect-y", &eRectY);
  bindFunction(env, "rect-w", &eRectW);
  bindFunction(env, "rect-h", &eRectH);
  bindFunction(env, "set-rect-y!", &eSetRectY);

  // UI functions
  bindFunction(env, "do-texture", &eDoTexture);
  bindFunction(env, "do-texture-box", &eDoTextureBox);
  bindFunction(env, "do-button", &eDoButton);
  bindFunction(env, "do-popup", &eDoPopup);

  bindFunction(env, "cid", &scID);
//   bindFunction(env, "count-modules", sCountModules);
//   bindFunction(env, "show-message", sShowMessage);
//   bindFunction(env, "enable-module", sEnableModule);
  
//   bindFunction(env, "get-credit-result", sGetCreditResult);
//   bindFunction(env, "get-flow", sGetFlow);
//   bindFunction(env, "get-positive-capacity", sGetPositiveCapacity);
  
  // Main
  bindFunction(env, "exit", &eExit);

  return env;
}

