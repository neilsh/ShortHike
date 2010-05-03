//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "Main.hpp"
#include "Entry.hpp"
#include "Timer.hpp"
#include "Platform.hpp"
#include "GamePlay.hpp"
#include "ModuleEditor.hpp"
#include "LocationEditor.hpp"

// TEMP
#include "file/CacheManager.hpp"

// #include "config/ConfigManager.hpp"
// #include "config/Config.hpp"
// #include "cache/CacheManager.hpp"
// #include "drm/License.hpp"
// #include "drm/LimitDialog.hpp"
// #include "drm/Patcher.hpp"
// #include "drm/ActivateDialog.hpp"
// #include "audio/SoundManager.hpp"
// #include "audio/Ambient.hpp"
// #include "script/FunctionExporters.hpp"
// #include "script/LuaManager.hpp"
// #include "script/StandardParseConsumer.hpp"
// #include "property/PrototypeManager.hpp"
// #include "property/DataFile.hpp"

// #include "rendering/Mesh.hpp"
// #include "rendering/Texture.hpp"

#include "game/Mission.hpp"
#include "game/Station.hpp"
#include "guilib/GUIManager.hpp"
#include "layers/BuildLayer.hpp"
#include "layers/ProfileLayer.hpp"
#include "rendering/Camera.hpp"
#include "rendering/RenderSystem.hpp"

#include "profiler/prof.h"

// #include "config/Player.hpp"

// #include "widgets/CollisionManager.hpp"
// #include "widgets/ControlDock.hpp"
// #include "widgets/Debrief.hpp"
// #include "widgets/DebugTool.hpp"
// #include "widgets/FileDialog.hpp"
// #include "widgets/MessageReader.hpp"
// #include "widgets/MissionSelector.hpp"
// #include "widgets/ModuleDock.hpp"
// #include "widgets/PlayerProfile.hpp"
// #include "widgets/ScriptMenu.hpp"
// #include "widgets/StatusReport.hpp"
// #include "widgets/TrackBallCamera.hpp"

// #include "orbit/Orbit.hpp"
// #include "screens/LoadingScreen.hpp"


// #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
// #include "Wininet.h"
// #include "DbgHelp.h"
// #endif

//--------------------------------------------------------------------------------
// Singleton implementation

Main*
rMain()
{
  static Main* mMain = new Main();
  return mMain;
}



//--------------------------------------------------------------------------------
// Initializing the main screen

Main::Main()
  : isRunning(false), mIsActive(false), mProfiling(false), mFrameTime(0.1), mModeID(MODE_EMPTY), mMode(NULL)
{
}


Main::~Main()
{
//   delete SoundManager::getSingletonPtr();
}

//--------------------------------------------------------------------------------
// Loads the default config file or creates a new configuration

// void
// Main::setupConfigManager()
// {
//   new PrototypeManager();
//   ConfigManager* configManager = new ConfigManager();
//   configManager->loadConfig("config.shd");
//   CHECK("Valid current config", ConfigManager::getCurrent() != NULL);

//   // Set up a license for this installation
//   if(ConfigManager::getCurrent()->getLicense() == NULL) {
//     License* license = new License(ConfigManager::getCurrent()->getDataFile());
//     ConfigManager::getCurrent()->setLicense(license);
//   }
//   // If initializing for the first time then we need to save
//   if(ConfigManager::getCurrent()->getLicense()->initialize()) {
//     ConfigManager::getSingleton().saveConfig();
//   }
// }

//--------------------------------------------------------------------------------


// bool
// Main::initialize()
// {
//   SET_TERM_HANDLER;

//   new Root("", "", ""); // Create OGRE Root singleton.  
//   printVersionInformation();
//   setupRenderSystems();
//   if(setupRenderWindow() == false) return false;  
//   setupLoadingScreen();
//   setupAudio();

  // Need to restart after recovering from an update
//   if(true == recoverUpdateInProgress())
//     return false;  

  // This is the main loading part
//   preLoad();
//   setupCollision();
//   setupScripting();
//   setupResources();
//   setupSingletonsAndModes();
//   setupCollisionObjects();
//   postLoad();
  
//   setupPlayer();
//   setupRunTimeLogging();

//   screenManager.setCurrentState(ORBIT_SCREEN);
//   return true;
//   return true;
// }




//--------------------------------------------------------------------------------
// Selecting render system

// void
// Main::setupRenderSystems()
// {
//   logEngineInfo("Main::setupRenderSystems");

//   Root* ogreRoot = Root::getSingletonPtr();
//   timer = PlatformManager::getSingleton().createTimer();

//   ogreRoot->loadPlugin("RenderSystem_GL");  

//   RenderSystemList* renderSystems = ogreRoot->getAvailableRenderers();
//   CHECK("Only a single RenderSystem available", renderSystems->size() == 1);
//   RenderSystem* openGLRenderSystem = renderSystems->at(0);

//   if(ConfigManager::getCurrent()->getDisableShaders() == true) {
//     openGLRenderSystem->setConfigOption("disableShaders", "true");
//   }  

//   ogreRoot->setRenderSystem(renderSystems->at(0));

//   try {
//     ogreRoot->loadPlugin("Plugin_CgProgramManager");  
//   }
//   catch(Exception& e) {
//     logEngineError(String("Unable to create CG Program manager RenderSystem: ") + e.getFullDescription());
//   }
// }

//--------------------------------------------------------------------------------

// #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
// BOOL CALLBACK MonitorInfoEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) 
// { 
//   vector<RECT>* monitorBounds = reinterpret_cast<std::vector<RECT>*>(dwData); 
//   monitorBounds->push_back(*lprcMonitor); 
//   return TRUE; 
// } 


// HWND applicationWindow;

// LRESULT WINAPI globalMessageProcedure( HWND window, UINT message, WPARAM wParam, LPARAM lParam )
// {
//   RenderWindow* renderWindow = (RenderWindow*)(GetWindowLongPtr(window, 0));
//   if(renderWindow != NULL) {
//     switch(message) {
//     case WM_ACTIVATE:
//       if(WA_INACTIVE == LOWORD(wParam))
//         renderWindow->setActive(false);
//       else
//         renderWindow->setActive(true);
//       break;
//     case WM_PAINT:
//       renderWindow->update();
//       break;
//     case WM_NCCALCSIZE:
//       // Prevent addition of caption and borders
//       return NULL;
//     }
//   }
//   return DefWindowProc(window, message, wParam, lParam);
// }
// #endif

// bool
// Main::setupRenderWindow()
// {
//   logEngineInfo("Main::setupRenderWindow");
//   Root* ogreRoot = Root::getSingletonPtr();

//   // Start of separation
//   ogreRoot->initialise(false);

//   NameValuePairList windowParams;
//   int windowWidth;
//   int windowHeight;
//   bool fullScreen;
  
// #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//   vector<RECT> monitorBounds; 
//   EnumDisplayMonitors(NULL, NULL, MonitorInfoEnumProc, reinterpret_cast<LPARAM>(&monitorBounds));
  
//   applicationWindow;
//   WNDCLASSEX windowClass;
//   ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
  
//   windowClass.cbSize = sizeof(WNDCLASSEX);
//   windowClass.style = CS_CLASSDC;
//   windowClass.lpfnWndProc = globalMessageProcedure;
//   windowClass.hInstance = GetModuleHandle(NULL);
//   windowClass.hIcon = LoadIcon(currentInstance, "SHORTHIKEICON");
//   windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
//   windowClass.cbWndExtra = sizeof(LONG_PTR);
//   windowClass.lpszClassName = "ShortHike";

//   RegisterClassEx(&windowClass);

//   RECT windowSize;
//   int displayMonitor = ConfigManager::getCurrent()->getDisplayMonitor();
//   displayMonitor = clamp(displayMonitor, 0, static_cast<int>(monitorBounds.size() - 1));
//   windowSize = monitorBounds[displayMonitor];

//   windowWidth = windowSize.right - windowSize.left;
//   windowHeight = windowSize.bottom - windowSize.top;
//   // Force smaller window
//   windowSize.left = 20;
//   windowSize.top = 20;
//   windowWidth = 1024;
//   windowHeight = 768;
// //  Force smaller window
// //   windowSize.left = 50;
// //   windowSize.top = 50;
// //   windowWidth = 800;
// //   windowHeight = 600;
//   fullScreen = false;
  
//   DWORD windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
//   applicationWindow = CreateWindow( "ShortHike", "ShortHike", 
//                                     windowStyle, windowSize.left, windowSize.top, windowWidth, windowHeight,
//                                     GetDesktopWindow(), NULL, windowClass.hInstance, NULL);

//   // Separating out OGRE
// //   windowParams["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)applicationWindow);  
// #endif

//   windowParams["FSAA"] = ConfigManager::getCurrent()->getAntiAliasing();
//   renderWindow = ogreRoot->createRenderWindow("REFACTOR OGRE", windowWidth, windowHeight, fullScreen, &windowParams);
//   renderWindow->setActive(true);
//   ogreRoot->getRenderSystem()->_initRenderTargets();

// //   SetWindowLongPtr(applicationWindow, 0, (LONG_PTR)renderWindow);
// //   CHECK("RenderWindow as handle", GetWindowLongPtr(applicationWindow, 0) != 0);
//   ShowWindow(applicationWindow, SW_SHOWNORMAL);
// //   UpdateWindow(applicationWindow);
//   CHECK("DX7RenderSystem creation succeeded", createDX7RenderSystem(applicationWindow, &mRenderSystem));
//   mCamera = new Camera();
//   return true;
//   return true;
// }

//--------------------------------------------------------------------------------

// void
// Main::setupLoadingScreen()
// {
//   GUIManager* guiManager = new GUIManager(renderWindow);

//   // Directory where to find loading data
//   ResourceGroupManager::getSingleton().addResourceLocation("assets/loading", "FileSystem", RESOURCE_GROUP_LOADING, false);

//   logEngineInfo("Main::setupRenderWindow - preparing LoadingScreen");
//   new LoadingScreen(this);
//   screenManager.addState(LOADING_SCREEN, LoadingScreen::getSingletonPtr());  
//   screenManager.setCurrentState(LOADING_SCREEN);
//   logEngineInfo("Main::setupRenderWindow - LoadingScreen initialized");
// }


//--------------------------------------------------------------------------------
// Create the audio system

// void
// Main::setupAudio()
// {
//   logEngineInfo("Main::setupAudio - Starting SoundManager");
//   SoundManager* soundManager = new SoundManager();
//   logEngineInfo("Main::setupAudio - Loading startup ambient file");
//   Ambient* loadingAmbient = new Ambient("LoadingAmbient.ogg", RESOURCE_GROUP_LOADING, 1.0);
//   logEngineInfo("Main::setupAudio - Adding ambient to SoundManager");
//   soundManager->addAmbient("loading/ambient", loadingAmbient);
//   logEngineInfo("Main::setupAudio - Activating ambient");
//   SoundManager::setAmbient("loading/ambient");
//   logEngineInfo("Main::setupAudio - DONE");
// }


//--------------------------------------------------------------------------------
// Check if updating is in progress and restart it in that case
bool
Main::recoverUpdateInProgress()
{
//   if(true == ConfigManager::getCurrent()->isUpdateInProgress()) {
//     // We still might run into a case where no update is needed
//     if(false == patcherUpdateAll()) return false;
//     // We recovered, so handle accordingly
//     return true;
//   }
//   // Normal startup
//   else {
//     return false;
//   }
  return false;
}



//--------------------------------------------------------------------------------

// void
// Main::preLoad()
// {
//   const int MAGIC_NUMBER_OF_TICKS = 405;
  
//   // Use presaved ticks data if available
//   logEngineInfo("Main::preLoad - finding number of ticks");
//   Config* currentConfig = ConfigManager::getSingleton().getCurrentConfig();
//   if(currentConfig->getLoadTicks().size() > 0)
//     LoadingScreen::getSingleton().startTask(L"Loading..", currentConfig->getLoadTicks()); 
//   else 
//     LoadingScreen::getSingleton().startTask(L"Loading..", MAGIC_NUMBER_OF_TICKS);
//   logEngineInfo("Main::preLoad - finding number of load ticks: DONE");
// }


//--------------------------------------------------------------------------------

// void
// Main::setupCollision()
// {
//   LoadingScreen::tick(L"CollisionManager");
//   new CollisionManager();
// }


//--------------------------------------------------------------------------------

// void
// Main::setupScripting()
// {
//   LoadingScreen::tick(L"LuaManager");
//   LuaManager* luaRoot = new LuaManager();
//   luaRoot->setParseConsumer(new StandardParseConsumer());
//   luaRoot->doFile("config.lua");
//   LoadingScreen::tick(L"MissionSelector");
//   new MissionSelector();
// }



//--------------------------------------------------------------------------------
// Setting up resources

// void
// Main::setupResources()
// {
//   ResourceGroupManager& groupManager = ResourceGroupManager::getSingleton();

//   // Default location to load from is assets dir
//   // This is added as a cache. The cache supports loading of .shi file that are mapped to either
//   // tga or .shc compressed files.
//   LoadingScreen::tick(L"Creating CacheManager");
//   new CacheManager();
//   LoadingScreen::tick(L"Adding assets/");
//   groupManager.addResourceLocation("assets", "Cache", RESOURCE_GROUP_MAIN, true);

//   // Load all zip files from "package/"
//   try {
//     LoadingScreen::tick(L"Adding packages");
//     Archive* packages = ArchiveManager::getSingleton().load("packages", "FileSystem");
//     logEngineInfo("Opening package zip files");
    
//     StringVectorPtr zipFiles = packages->find("*.zip");
//     for(StringVector::const_iterator fileI = zipFiles->begin(); fileI != zipFiles->end(); ++fileI) {
//       string fileName = *fileI;
//       LoadingScreen::tick(wstring(L"Adding package file: ") + stringToWString(fileName));
//       logEngineInfo(string("Package: ") + fileName);
//       try {
//         groupManager.addResourceLocation(string("packages/") + fileName, "Zip", RESOURCE_GROUP_MAIN, true);
//       }
//       catch(Exception& error) {
//         ostringstream errorStream;
//         errorStream << "Error loading package: " << fileName << endl
//                     << error.getFullDescription();
//         logEngineError(errorStream.str());
//       }      
//     }
//   }
//   catch(Exception& error) {
//     ostringstream errorStream;
//     errorStream << "Error opening packages directory: " << error.getFullDescription();
//     logEngineError(errorStream.str());
//   }

//   LoadingScreen::tick(L"Adding local");
//   try {
//     groupManager.addResourceLocation("local", "FileSystem", RESOURCE_GROUP_MAIN, true);    
//   }
//   catch(Exception& error) {
//     ostringstream errorStream;
//     errorStream << "Error opening local directory: " << error.getFullDescription();
//     logEngineError(errorStream.str());
//   }

//   LoadingScreen::tick(L"Initializing resource groups");
//   groupManager.initialiseAllResourceGroups();
//   LoadingScreen::tick(L"Loading all resources");
// #ifndef _DEBUG
//   groupManager.loadResourceGroup(RESOURCE_GROUP_MAIN);
// #endif
// }



//--------------------------------------------------------------------------------

// void
// Main::setupSingletonsAndModes()
// {
//   LoadingScreen::tick(L"ControlDock");
//   new ControlDock();
//   LoadingScreen::tick(L"ModuleDock");
//   new ModuleDock();
//   LoadingScreen::tick(L"FileDialog");
//   new FileDialog();
//   LoadingScreen::tick(L"LimitDialog");
//   new LimitDialog();
//   LoadingScreen::tick(L"ActivateDialog");
//   new ActivateDialog();
//   LoadingScreen::tick(L"MessageReader");
//   new MessageReader();
//   LoadingScreen::tick(L"StatusReport");
//   new StatusReport();
//   LoadingScreen::tick(L"ScriptMenu");
//   new ScriptMenu();
//   LoadingScreen::tick(L"Debrief");
//   new Debrief();
//   LoadingScreen::tick(L"PlayerProfile");
//   new PlayerProfile();
//   LoadingScreen::tick(L"DebugTool");
//   new DebugTool();

//   // Screens
//   LoadingScreen::tick(L"Orbit");
//   orbitScreen = new Orbit(this);
//   screenManager.addState(ORBIT_SCREEN, orbitScreen);
// }

//--------------------------------------------------------------------------------

// void
// Main::setupCollisionObjects()
// {
//   CollisionManager* cManager = CollisionManager::getSingletonPtr();

//   cManager->addMesh("user_interface/port.mesh");
//   cManager->addMesh("user_interface/mesh/ModuleMarker.mesh");
// }


//--------------------------------------------------------------------------------

// void
// Main::postLoad()
// {
//   // Store ticks into config
//   Config* currentConfig = ConfigManager::getSingleton().getCurrentConfig();
//   LoadingScreen::getSingleton().endTask();
//   currentConfig->setLoadTicks(LoadingScreen::getSingleton().getNormalizedTicks());
//   ConfigManager::getSingleton().saveConfig();
// }


//--------------------------------------------------------------------------------

// void
// Main::setupPlayer()
// {
//   ConfigManager::getSingleton().loadPlayer("player.shd");
// }


//--------------------------------------------------------------------------------

// void
// Main::setupRunTimeLogging()
// {
//   // FIXME: zzz add some runtime logging levels here ..
// }

// --------------------------------------------------------------------------------

void
Main::setMode(Mode newMode)
{
  if(mMode != NULL)
    mMode->exitState();
  mModeID = newMode;

  if(MODE_EMPTY == mModeID)
    mMode = NULL;
  else if(MODE_GAMEPLAY == mModeID)
    mMode = rGamePlay();
//   else if(MODE_MISSION_EDITOR == mModeID)
//     mMode = NULL;
  else if(MODE_MODULE_EDITOR == mModeID)
    mMode = rModuleEditor();
  else if(MODE_LOCATION_EDITOR == mModeID)
    mMode = rLocationEditor();
  
  if(mMode != NULL)
    mMode->enterState();
}




//================================================================================
// Main Loop

void
reportTime(Timer* timer, string checkPoint)
{
  unsigned long timePoint = timer->check(1000);
  ostringstream ossStartupInfo;
  ossStartupInfo << checkPoint << fixed << setprecision(0) << timePoint << "ms";
  logEngineInfo(ossStartupInfo.str());
}


const int MAGIC_FOCUS_LOST_DELAY = 10;

int
Main::mainLoop()
{
  Prof_update(1);
  
  Timer* timer = rPlatform()->createTimer();
  isRunning = true;
  timer->start();

  double delaySleepTrigger = 0.005;
  double latentDelay = 0;
  const double MAGIC_MINIMUM_UPDATE_TIME = 0.005;
  const double MAGIC_UPDATE_TIME_PRIMING = 0.1;
  double updateTime = MAGIC_UPDATE_TIME_PRIMING;
  int focusLostDelay = MAGIC_FOCUS_LOST_DELAY;

  // Fix better lighting ..
  rRenderSystem()->enableSunLight(true);

  // DEBUG
  rGamePlay()->setMission(new Mission());
  setMode(MODE_GAMEPLAY);
//    setMode(MODE_LOCATION_EDITOR);

  render();

  reportTime(timer, "ShortHike startup: ");
  
// #pragma warning(disable: 4127)
//   if(true)
//     return 0;

  //------------------------------------------------------------
  // The delay stuff essentially throttles the update loop to a
  // max of 200FPS. It's a solution to Case 19
  Prof_update(1);
  timer->start();
  setProfiling(true);
  while(isRunning) {
    if(latentDelay >= delaySleepTrigger) {
      Sleep(static_cast<int>(latentDelay * 1000.0));
      latentDelay = 0.0f;
    }
    // fixed updating
    Prof_update(mProfiling);
    
    unsigned long currentTimeMicroS = timer->check(1000000);
    timer->start();
    double timeStep = static_cast<double>(currentTimeMicroS) * 0.000001;
    if(timeStep < MAGIC_MINIMUM_UPDATE_TIME) {
      latentDelay += (MAGIC_MINIMUM_UPDATE_TIME - timeStep);
    }
    
    mFrameTime = timeStep;
    
    rPlatform()->pumpMessages();
    
    if(mIsActive || focusLostDelay > 0) {
      // Only filter the updateTime when we are running
      updateTime = updateTime * 0.90f + timeStep * 0.10f;
      dispatchTime(updateTime);

      focusLostDelay--;

      render();
    }
    if(mIsActive)
      focusLostDelay = MAGIC_FOCUS_LOST_DELAY;

//     if(renderWindow->isClosed())
//       isRunning = false;

    // Forced abort for testing
//    isRunning = false;
  }  

  return 0;
}

//--------------------------------------------------------------------------------

void
Main::renderAndUpdate()
{
  rPlatform()->pumpMessages();
  render();
}


void
Main::render()
{
  Prof(render);
  if(rRenderSystem()->beginScene()) {
    rGUIManager()->render(rRenderSystem());
    rRenderSystem()->endScene();
  }
}

//--------------------------------------------------------------------------------

void
Main::setActive(bool isActive)
{
  mIsActive = isActive;
}

//--------------------------------------------------------------------------------
// Dispatch time to various listeners around the code

void
Main::dispatchTime(float timeStep)
{
  Prof(dispatch_time);
  mMode->update(timeStep);
//   mCurrentMission->updateSimulation();
  rGUIManager()->update(timeStep);

//   SoundManager::getSingletonPtr()->update(timeStep);
//   screenManager.getCurrentState()->update(timeStep);
//   ConfigManager::getCurrent()->getLicense()->update(timeStep);
//   ConfigManager::getPlayer()->mPlayTime += timeStep;
}

//--------------------------------------------------------------------------------

void
Main::quitApplication()
{
  isRunning = false;
}


// bool
// Main::frameStarted(const FrameEvent& event)
// {
//   return isRunning;
// }


//--------------------------------------------------------------------------------

// void
// Main::reload()
// {
//   logEngineInfo("--------------------------------------------------------------------------------");
//   logEngineInfo(" Starting resource reload");
//   HighLevelGpuProgramManager::getSingleton().reloadAll();
//   logEngineInfo(" Resource reload DONE");
//   logEngineInfo("--------------------------------------------------------------------------------");
// }


