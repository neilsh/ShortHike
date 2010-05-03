//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import BuildInfo;
private import Platform;
private import Version;
private import RenderSystem;
private import GUIManager;
private import ShellScreen;
private import StationScreen;
private import ToolScreen;
private import Camera;

private import SocketREPL;
private import Value;
private import Export;
private import Library;

// Debug
// private import CacheManager;
// private import Prototype;
// private import Mesh;
// private import Texture;


// --------------------------------------------------------------------------------
// Singleton implementation

private Main singletonMain;

Main
rMain()
{
  if(null is singletonMain)
    singletonMain = new Main();
  return singletonMain;
}

private Environment singletonEnv;

Environment
rEnv()
{
  if(null is singletonEnv)
    singletonEnv = standardEnvironment();
  return singletonEnv;
}

// --------------------------------------------------------------------------------

class Main
{

  //================================================================================
  // Main Loop
  
  // DEBUG
  const int MAGIC_FOCUS_LOST_DELAY = 10;

  void
  mainLoop()
  {
    //     Prof_update(1);

    printVersionInformation();

    Timer timer = rPlatform().createTimer();
    mIsRunning = true;
    timer.start();

    // Initialize the Scheme environment
    sLoad(rEnv(), "scm/entry.scm");
    mSocketREPL = new SocketREPL(rEnv()); 

    //     debug {
//     logInfo << " Running unit tests.." << endl;
//     runTestFixtures();
    //     }
    
    double delaySleepTrigger = 0.005;
    double latentDelay = 0;
    const double MAGIC_MINIMUM_UPDATE_TIME = 0.005;
    const double MAGIC_UPDATE_TIME_PRIMING = 0.025;
    double updateTime = MAGIC_UPDATE_TIME_PRIMING;
    
    mScreen = Screen.STATION;

    // DEBUG
    mCamera = rRenderSystem().createCamera();
    mMission = new Mission();
    // END DEBUG
 
    render();
    
    long timePoint = timer.check(1000);
    logInfo() << "ShortHike startup: " << timePoint << "ms" << endLog;
    
    //------------------------------------------------------------
    // The delay stuff essentially throttles the update loop to a
    // max of 200FPS. It's a solution to Case 19
    //     Prof_update(1);
    timer.start();
    setProfiling(true);
    while(mIsRunning) {
      if(latentDelay >= delaySleepTrigger) {
        Sleep(cast(uint)(latentDelay * 1000.0));
        latentDelay = 0.0f;
      }
      // fixed updating
      //       Prof_update(mProfiling);
      
      long currentTimeMicroS = timer.check(1000000);
      timer.start();
      double timeStep = cast(double)currentTimeMicroS * 0.000001;
      if(timeStep < MAGIC_MINIMUM_UPDATE_TIME) {
        latentDelay += (MAGIC_MINIMUM_UPDATE_TIME - timeStep);
      }
      
      mFrameTime = timeStep;
      
      mSocketREPL.processFrame();
      rGUIManager().updateFrame(timeStep);
      rPlatform().pumpMessages();
      
      if(mIsActive || mFocusLostDelay > 0) {
        // Only filter the updateTime when we are running
        updateTime = updateTime * 0.90f + timeStep * 0.10f;
        mFocusLostDelay--;
        
        //   SoundManager::getSingletonPtr().update(timeStep);
        render(updateTime);
      }
      if(mIsActive)
        mFocusLostDelay = MAGIC_FOCUS_LOST_DELAY;
      
      //     if(renderWindow.isClosed())
      //       mIsRunning = false;
      
      // Forced abort for testing
      //    mIsRunning = false;
    }  
  }

  //--------------------------------------------------------------------------------

  void render(float timeStep = 0)
  {
    RenderSystem renderSystem = rRenderSystem();
    //     Prof(render);
    rIncreaseFrame();
    if(renderSystem.beginScene()) {
      if(mScreen == Screen.SHELL) {
        mScreen = doShellScreen(timeStep, renderSystem, mMission.getLocation());
      }
      else if(mScreen == Screen.STATION) {
        mScreen = doStationScreen(timeStep, renderSystem, mMission, mCamera);
      }
//       else if(mScreen == Screen.MAP) {
//         mScreen = doMapScreen(timeStep, renderSystem, mCamera);
//       }
      else if(mScreen == Screen.TOOLS) {
        mScreen = doToolScreen(timeStep, renderSystem);
      }      
      rRenderSystem().endScene();
    }
  }

  //--------------------------------------------------------------------------------
  
  void renderNextFrame()
  {
    mFocusLostDelay = 1;
  }
  

  void setActive(bool isActive)
  {
    mIsActive = isActive;
  }  

  void setProfiling(bool profiling)
  {
    mProfiling = profiling;
  }
  
  void exit()
  {
    mIsRunning = false;
  }
  

  // --------------------------------------------------------------------------------

private:
  bool mIsRunning = false;
  bool mIsActive = false;
  bool mProfiling = false;
  float mFrameTime = 0.1;
  int mFocusLostDelay = MAGIC_FOCUS_LOST_DELAY;

  Screen mScreen;

  Mission mMission;
  Camera mCamera;

  SocketREPL mSocketREPL;
}

// --------------------------------------------------------------------------------


Value
eExit(Environment env, Value[] args)
{
  rMain().exit();
  return rsUndefined();
}


//--------------------------------------------------------------------------------
// Prints version information about this build of ShortHike

void
printVersionInformation()
{
  logInfo() << "--------------------------------------------------------------------------------" << endl;
  
  // Name and version
  {
    logInfo() << " ShortHike (";
    debug {
      logInfo() << "DEBUG ";
    }
    
    version(Unstable) {
      logInfo() << "Unstable)";
    }
    else {
      logInfo() << "Stable)";
    }
    
    logInfo() << " " << BUILD_VERSION << " " << BUILD_REVISION << endl;
  }  

  logInfo() << "  Build date: " << BUILD_DATE << endl;
  
  
  logInfo() << "--------------------------------------------------------------------------------" << endLog;
}



