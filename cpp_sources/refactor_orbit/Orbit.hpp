//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#ifndef USER_INTERFACE_ORBIT_SCREEN_HPP
#define USER_INTERFACE_ORBIT_SCREEN_HPP

#include "widgets/Screen.hpp"
#include "guilib/UserEventListener.hpp"
#include "guilib/ActionListener.hpp"

class Main;
class Station;
class Module;
class PlanetMesh;
class TrackBallCamera;
class OrbitState;
class OrbitMap;
class LayerDock;
class Widget;
class Label;
class Frame;

class Orbit : public Screen, public ActionListener
{
  friend ostream& operator<<(ostream& out, Orbit*);

  friend class OrbitState;
  friend class OrbitReadyState;
  friend class OrbitAddState;
  friend class OrbitSelectedState;
  friend class DirectManipulation;
public:
  Orbit(Main*);
  
  virtual void enterState();  
  virtual void exitState();

  virtual bool mouseDownEvent(int button, int x, int y);
  virtual bool mouseUpEvent(int button, int x, int y);
  virtual bool mouseClickEvent(int buttonID, unsigned clickCount, int x, int y);
  virtual void mouseMotionEvent(int x, int y, float deltaX, float deltaY, float deltaWheel);
  virtual bool keyboardEvent(char keyCode, bool pressed);

  virtual void update(float deltaT);

  virtual void actionPerformed(Widget* source);

  int getMeshGeneration() {return meshGeneration;}
  Degree getLongitude(float secondsAgo = 0.0f);
  Degree getLatitude(float secondsAgo = 0.0f);

  void setStation(Station* newStation);
  Station* getStation();
  void toggleModuleDock();
  void toggleBuildModule();
  void setPlanetResolution(int gridSize, bool reloadData = true);

  virtual bool requireExclusiveInput() {return mExclusiveInput;}
  virtual SceneManager* getSceneManager() {return sceneManager;}

  enum StateType {
    READY_STATE,
    ADD_STATE,
    SELECTED_STATE
  };

  void enterReadyState();
  void randomizeOrbit(bool reportProgress = false);

  Matrix4 getViewMatrix();

  TrackBallCamera* trackBallCamera;
private:

  Main* main;
  OctreeSceneManager* sceneManager;
  SceneNode* rootNode;
  
  bool mExclusiveInput;
  OrbitReadyState* readyState;
  OrbitAddState* addState;
  OrbitSelectedState* selectedState;
  StateManager<OrbitState> stateManager;

  Label* stationInfo;
  Label* layerInfo;
  Label* toolInfo;

  OrbitMap* orbitMap;

  Widget* interfaceContainer;
  LayerDock* layerDock;

  Label* mDebugLabel;

  Module* quickPrototypes[5];

  //----------------------------------------
  // Station variables

  Station* station;

  int meshGeneration;
  PlanetMesh* planetFrontBuffer;
  PlanetMesh* planetBackBuffer;

//   Vector orbitalAxis;
//   Vector observerPosition;
//   Vector initialObserverPosition;
  Degree orbitInclination;
  Degree orbitAscending;
  Degree orbitAngle;

  Real orbitTime;
  Real frameTime;
};

ostream& operator<<(ostream& out, Orbit*);

 
#endif
