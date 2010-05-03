//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef SHORTHIKE_ROOT_HPP
#define SHORTHIKE_ROOT_HPP

// SH_ROOT
// include functions like:
// rGUILib() that access singleton instances. Singletons only created when first accessed ..

// This file just contains the function declarations. Definitions are in each separate location.


class Platform;
class RenderSystem;
class Main;
class GUIManager;
class CacheManager;
class Repository;
class UpdateManager;

extern Platform* rPlatform();
extern RenderSystem* rRenderSystem();
extern Main* rMain();
extern GUIManager* rGUIManager();
extern CacheManager* rCacheManager();
extern Repository* rRepository();
extern UpdateManager* rUpdateManager();

// --------------------------------------------------------------------------------
// Modes

class GamePlay;
class MissionEditor;
class ModuleEditor;
class LocationEditor;

extern GamePlay* rGamePlay();
extern MissionEditor* rMissionEditor();
extern ModuleEditor* rModuleEditor();
extern LocationEditor* rLocationEditor();

// --------------------------------------------------------------------------------
// Layers

class BuildLayer;
class CameraLayer;
class InfoBrowser;
class KeyboardLayer;
class ModuleLayer;
class PrototypeLayer;
class ProfileLayer;
class SkyLayer;
class StationLayer;
class StatusLayer;

extern BuildLayer* rBuildLayer();
extern CameraLayer* rCameraLayer();
extern InfoBrowser* rInfoBrowser();
extern KeyboardLayer* rKeyboardLayer();
extern ModuleLayer* rModuleLayer();
extern PrototypeLayer* rPrototypeLayer();
extern ProfileLayer* rProfileLayer();
extern SkyLayer* rSkyLayer();
extern StationLayer* rStationLayer();
extern StatusLayer* rStatusLayer();

// --------------------------------------------------------------------------------
// Defined here

extern void printVersionInformation();

#endif
