//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "main/Main.hpp"

#include "Orbit.hpp"
#include "game/Aspects.hpp"
#include "game/Module.hpp"
#include "game/Port.hpp"
#include "game/Station.hpp"
#include "property/PrototypeManager.hpp"

#include "widgets/TrackBallCamera.hpp"
#include "widgets/ModuleDock.hpp"
#include "widgets/ControlDock.hpp"

#include "guilib/FrameBackground.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/Label.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/BorderLayout.hpp"
#include "audio/SoundManager.hpp"
#include "screens/LoadingScreen.hpp"

#include "PlanetMesh.hpp"
#include "OrbitReadyState.hpp"
#include "OrbitAddState.hpp"
#include "OrbitSelectedState.hpp"
#include "OrbitMap.hpp"
#include "LayerDock.hpp"

//--------------------------------------------------------------------------------
// Magic constants

const float ISS_ALTITUDE = 400;
const float ISS_PERIOD = 90.0f;
const float EARTH_RADIUS = 6378.1; // Earth

const Real PLANETMESH_AHEAD_INITIAL = 0.5;
const Real PLANETMESH_AHEAD_NORMAL = 1.5;
const int MAGIC_DEFAULT_PLANET_RESOLUTION = 4;

//--------------------------------------------------------------------------------

Orbit::Orbit(Main* _main)
  : main(_main), planetFrontBuffer(NULL), planetBackBuffer(NULL), frameTime(0.1), meshGeneration(0), station(NULL), mExclusiveInput(false)
{
  for(int iProt = 0; iProt < 5; ++iProt) quickPrototypes[iProt] = NULL;

  LoadingScreen::tick(L"Orbit::SceneManager");
  Root& ogreRoot = Root::getSingleton();
  sceneManager = static_cast<OctreeSceneManager*>(ogreRoot.getSceneManager(ST_GENERIC));
  sceneManager->_setDestinationRenderSystem(ogreRoot.getRenderSystem());
  sceneManager->setSkyBox(true, "ShortHike/Orbit/Skybox_Stars");
  // Make this a display option
//   sceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
  sceneManager->setAmbientLight(ColourValue(0.35, 0.35, 0.35));

  rootNode = sceneManager->getRootSceneNode();

  LoadingScreen::tick(L"Orbit::Lights");
  Light* sunLight = sceneManager->createLight("SunLight");
  sunLight->setType(Light::LT_DIRECTIONAL);
  Vector sunDirection(-80, -4, -100);
  sunDirection.normalise();
  sunLight->setPosition(80, 4, 100);
  sunLight->setDirection(sunDirection);
  sunLight->setDiffuseColour(.9f, .9f, .9f);
  sunLight->setSpecularColour(1.0f, 1.0f, 1.0f);
  rootNode->attachObject(sunLight);

  Light* earthLight = sceneManager->createLight("EarthLight");
  earthLight->setType(Light::LT_DIRECTIONAL);
  Vector earthDirection(0, 0, 50);
  earthDirection.normalise();
  earthLight->setPosition(0, 0, -50);
  earthLight->setDirection(earthDirection);
  earthLight->setDiffuseColour(0.55f, 0.55f, 0.70f);
  earthLight->setSpecularColour(0, 0, 0);
  rootNode->attachObject(earthLight);

  trackBallCamera = new TrackBallCamera(sceneManager);

  LoadingScreen::tick(L"Orbit::setPlanetResolution");
  setPlanetResolution(MAGIC_DEFAULT_PLANET_RESOLUTION, false);

  LoadingScreen::tick(L"Orbit::UI - BottomFrame");
  
  // New UI
  {
    interfaceContainer = new Widget();
    Widget* rootContainer = rGUIManager()->getRootContainer();
    rootContainer->add(interfaceContainer);
    interfaceContainer->setPosition(0, 0);
    interfaceContainer->setSize(rootContainer->getSize());
    GridLayout* interfaceLayout = new GridLayout(interfaceContainer, 24, 18);    
    interfaceLayout->set_padding(0);
    interfaceLayout->set_borders(0);
    
    {
      stationInfo = new Label(L"Station information");
      stationInfo->setVerticalAlignment(Label::TOP);
      
      Frame* stationInfoFrame = new Frame("Orbit/StationInfoFrame");
      stationInfoFrame->setLayoutManager(new BorderLayout());
      stationInfoFrame->setBackground(new FrameBackground());
      stationInfoFrame->add(stationInfo, "center");
      interfaceLayout->add(stationInfoFrame, 0, 15, 4, 3);
    }
    
    {
      toolInfo = new Label(L"Tool/Module information");
      toolInfo->setVerticalAlignment(Label::TOP);

      Frame* toolInfoFrame = new Frame("Orbit/ToolInfoFrame");
      toolInfoFrame->setLayoutManager(new BorderLayout());
      toolInfoFrame->setBackground(new FrameBackground());
      toolInfoFrame->add(toolInfo, "center");

      interfaceLayout->add(toolInfoFrame, 20, 15, 4, 3);
    }    
    
    {
      int mapWidth = 300;
      int mapHeight = 150;
      orbitMap = new OrbitMap(this, "user_interface/EarthMap.png");
      orbitMap->setPreferredSize(mapWidth, mapHeight);
      layerDock = new LayerDock(this, orbitMap);
    }

    interfaceLayout->add(ModuleDock::getSingletonPtr(), 0, 1, 4, 14);
    interfaceLayout->add(ControlDock::getSingletonPtr(), 0, 0, 24, 1);
    interfaceLayout->add(layerDock, 4, 15, 16, 3);

    // DEBUG:
//     Frame* debugFrame = new Frame("DEBUGFRAME");
//     debugFrame->setLayoutManager(new BorderLayout());
//     mDebugLabel = new Label();
//     debugFrame->add(mDebugLabel, "center");
//     interfaceLayout->add(debugFrame, 18, 1, 6, 15);
  }

  LoadingScreen::tick(L"Orbit::UI - States");
  readyState = new OrbitReadyState(this);
  addState = new OrbitAddState(this);
  selectedState = new OrbitSelectedState(this);

  stateManager.addState(READY_STATE, readyState);
  stateManager.addState(ADD_STATE, addState);
  stateManager.addState(SELECTED_STATE, selectedState);

  LoadingScreen::tick(L"Randomizing Orbit");
  randomizeOrbit(true);
}


//--------------------------------------------------------------------------------

void
Orbit::enterState()
{
    Screen::enterState();

//     Viewport* viewport = main->getRenderWindow()->addViewport(trackBallCamera->getCamera());
//     viewport->setBackgroundColour(ColourValue(0,0,0.4f));

//     rGUIManager()->setSceneManager(sceneManager, trackBallCamera->getCamera());
    rGUIManager()->setRootListener(this);

    ModuleDock::getSingleton().setActionListener(this);
    interfaceContainer->setVisible(true);
    interfaceContainer->layout();
    ControlDock::getSingleton().setVisible(true);

    stateManager.setCurrentState(READY_STATE);
    SoundManager::setAmbient("orbit/ambient");
}


void
Orbit::exitState()
{
  stateManager.clearState();
  
  interfaceContainer->setVisible(false);
  ModuleDock::getSingleton().setActionListener(NULL);

  rGUIManager()->setRootListener(NULL);
    
//   main->getRenderWindow()->removeAllViewports();
  
  setStation(NULL);

  Screen::exitState();
}


//--------------------------------------------------------------------------------

void
Orbit::enterReadyState()
{
  stateManager.setCurrentState(READY_STATE);
}


//--------------------------------------------------------------------------------

Matrix4
Orbit::getViewMatrix()
{
  return trackBallCamera->getViewMatrix();
}


//--------------------------------------------------------------------------------

void
Orbit::randomizeOrbit(bool reportProgress)
{
  orbitTime = 0;

  orbitInclination = fnrand() * 90.0f;
  orbitAscending = fnrand() * 180.0f;
  orbitAngle = 0;

  LoadingScreen::tick(L"Orbit::FrontBuffer - startLoad");

  planetFrontBuffer->setObserverPosition(orbitInclination, orbitAscending, EARTH_RADIUS + ISS_ALTITUDE, orbitAngle);
  planetFrontBuffer->startLoad(PLANETMESH_AHEAD_INITIAL);
  LoadingScreen::tick(L"Orbit::FrontBuffer - loadAll");
  planetFrontBuffer->loadAll(reportProgress);
  planetFrontBuffer->setVisible(true);
  
  LoadingScreen::tick(L"Orbit::BackBuffer - startLoad");
  planetBackBuffer->setObserverPosition(orbitInclination, orbitAscending, EARTH_RADIUS + ISS_ALTITUDE, orbitAngle);
  planetBackBuffer->startLoad(PLANETMESH_AHEAD_NORMAL);
  planetBackBuffer->setVisible(false);

  LoadingScreen::tick(L"Orbit::OrbitMap - rePlot");
  orbitMap->rePlot();
}


//--------------------------------------------------------------------------------

void
Orbit::update(float timeStep)
{
  orbitTime += timeStep;

  orbitAngle = Degree(360.0f * orbitTime / (ISS_PERIOD * 60.0f));
  
  planetFrontBuffer->setObserverPosition(orbitInclination, orbitAscending, EARTH_RADIUS + ISS_ALTITUDE, orbitAngle);
  planetBackBuffer->setObserverPosition(orbitInclination, orbitAscending, EARTH_RADIUS + ISS_ALTITUDE, orbitAngle);

  if(planetBackBuffer->loadIncremental() == PlanetMesh::READY) {
    if(planetFrontBuffer->movingAway() == true
       && planetFrontBuffer->isWithinEffectiveBounds() == false) {
      PlanetMesh* temp = planetFrontBuffer;
      planetFrontBuffer = planetBackBuffer;
      planetBackBuffer = temp;
      
      planetBackBuffer->setObserverPosition(orbitInclination, orbitAscending, EARTH_RADIUS + ISS_ALTITUDE, orbitAngle);
      planetBackBuffer->startLoad(PLANETMESH_AHEAD_NORMAL);
      
      planetBackBuffer->setVisible(false);
      planetFrontBuffer->setVisible(true);
      
      logEngineInfo("Switching PlanetMesh");
      meshGeneration++;
    }
  }
  planetFrontBuffer->synchronizeCamera();

  if(station != NULL) {
    wostringstream stationInfoStream;
    stationInfoStream << "Modules: " << station->getModuleCount() << endl
                      << "Mass: " << station->getMass() << " ton" << endl << endl;
    
    Aspects* aspects = rMain()->getAspects();
    if(aspects->isCredits() && (aspects->isIncome() || aspects->isExpenses())) {
      stationInfoStream << "- Finances (rolling year)" << endl;
      if(aspects->isIncome())
        stationInfoStream << L"Income: $" << fixed << setprecision(1) << station->getTotalIncome() << "M" << endl;
      if(aspects->isExpenses())
        stationInfoStream << L"Expenses: $" << station->getTotalExpenses() << "M" << endl;
    
      Real result = station->getTotalIncome() - station->getTotalExpenses();
      if(result < 0) {
        stationInfoStream << L"LOSS: $" << result << "M" << endl;
      }
      else {
        stationInfoStream << L"Profit: $" << result << "M" << endl;
      }
    }
    
    stationInfo->setLabel(stationInfoStream.str());
  }  
  
//   const RenderTarget::FrameStats& stats = main->getRenderWindow()->getStatistics();
//   stationInfoStream << L" (" << setprecision(1) << fixed 
//                     << L"cFPS: " << stats.lastFPS 
//                     << L" aFPS: " << stats.avgFPS
//                     << L" tri: " << stats.triangleCount
//                     << L" mod: " << station->getModuleCount()
//                     << L")";
  

  wostringstream toolInfoStream;
  stateManager.getCurrentState()->outputString(toolInfoStream);
  toolInfo->setLabel(toolInfoStream.str());

  stateManager.getCurrentState()->update(timeStep);

  // DEBUG
//   ostringstream ossDebugStream;
//   ossDebugStream << this;
//   mDebugLabel->setLabel(stringToWString(ossDebugStream.str()));
}


//--------------------------------------------------------------------------------

void
Orbit::actionPerformed(Widget*)
{
  stateManager.setCurrentState(ADD_STATE);
  addState->setPrototype(const_cast<Module*>(ModuleDock::getSingleton().getCurrentPrototype()));
}



//--------------------------------------------------------------------------------
// Delegate events to current State

bool
Orbit::mouseDownEvent(int button, int x, int y)
{
  mExclusiveInput = true;
  return stateManager.getCurrentState()->mouseDownEvent(button, x, y);
}

bool
Orbit::mouseUpEvent(int button, int x, int y)
{
  mExclusiveInput = false;
  return stateManager.getCurrentState()->mouseUpEvent(button, x, y);
}

bool
Orbit::mouseClickEvent(int button, unsigned clickCount, int x, int y)
{
  return stateManager.getCurrentState()->mouseClickEvent(button, clickCount, x, y);
}

void
Orbit::mouseMotionEvent(int x, int y, float deltaX, float deltaY, float deltaWheel)
{
  return stateManager.getCurrentState()->mouseMotionEvent(x, y, deltaX, deltaY, deltaWheel);
}


bool
Orbit::keyboardEvent(char keyCode, bool pressed)
{
  return stateManager.getCurrentState()->keyboardEvent(keyCode, pressed);
}


//--------------------------------------------------------------------------------
// Utility functions

// FIXME: Remove or refactor to separate class

Degree
Orbit::getLongitude(float )
{
  return planetFrontBuffer->getLongitude();
}

Degree
Orbit::getLatitude(float )
{
  return planetFrontBuffer->getLatitude();
}

//--------------------------------------------------------------------------------
// Station management

void
Orbit::setStation(Station* _station)
{
  if(station != NULL) {
    station->detachStation();
  }
  
  station = _station;

  if(station != NULL) {
    station->attachStation(rootNode);
  }
  ControlDock::getSingleton().setCurrentStation(station);
}

Station*
Orbit::getStation()
{
  return station;
}


//--------------------------------------------------------------------------------

void
Orbit::toggleModuleDock()
{
    ModuleDock& prototypeSelector = ModuleDock::getSingleton();
    prototypeSelector.setVisible(!prototypeSelector.getVisible());
}

//--------------------------------------------------------------------------------

void
Orbit::toggleBuildModule()
{
  const Module* lastPrototype = addState->getLastPrototype();
  if(NULL != lastPrototype) {
    if(stateManager.getCurrentState() != addState) {
      stateManager.setCurrentState(addState);
    }
    addState->setPrototype(const_cast<Module*>(lastPrototype));
  }
}


//--------------------------------------------------------------------------------

void
Orbit::setPlanetResolution(int gridSize, bool reloadData)
{
  LoadingScreen::tick();
  if(planetFrontBuffer != NULL)
    delete planetFrontBuffer;
  if(planetBackBuffer != NULL)
    delete planetBackBuffer;

  // Create two detail mesh planets for updating purpose
  LoadingScreen::tick(L"Initalizing FrontBuffer");
  planetFrontBuffer = new PlanetMesh(sceneManager, "frontBuffer", "sites/earth_diffuse.ecw",
                                     "sites/earth_topo.ecw", EARTH_RADIUS, trackBallCamera->getCamera(), gridSize);
  logEngineInfo("PlanetMesh frontBuffer initialized");
  
  LoadingScreen::tick(L"Initializing BackBuffer");
  planetBackBuffer = new PlanetMesh(sceneManager, "backBuffer", "sites/earth_diffuse.ecw",
                                    "sites/earth_topo.ecw", EARTH_RADIUS, trackBallCamera->getCamera(), gridSize);
  logEngineInfo("PlanetMesh backBuffer initialized");  

  LoadingScreen::tick();
  planetFrontBuffer->setObserverPosition(orbitInclination, orbitAscending, EARTH_RADIUS + ISS_ALTITUDE, orbitAngle);
  planetFrontBuffer->startLoad(PLANETMESH_AHEAD_INITIAL);
  if(reloadData)
    planetFrontBuffer->loadAll();
  planetFrontBuffer->setVisible(true);

  LoadingScreen::tick();
  planetBackBuffer->setObserverPosition(orbitInclination, orbitAscending, EARTH_RADIUS + ISS_ALTITUDE, orbitAngle);
  planetBackBuffer->startLoad(PLANETMESH_AHEAD_NORMAL);
  planetBackBuffer->setVisible(false);
}



//--------------------------------------------------------------------------------
// Output

ostream& operator<<(ostream& out, Orbit* orbitScreen)
{
  out << "Longitude: " << orbitScreen->getLongitude().valueDegrees() << endl
      << "Latitude: " << orbitScreen->getLatitude().valueDegrees() << endl
      << "PlanetMesh: " << orbitScreen->getMeshGeneration() << endl << endl
      << "[FrontBuffer] " << orbitScreen->planetFrontBuffer << endl
      << "[BackBuffer] " << orbitScreen->planetBackBuffer << endl;
  return out;
}

