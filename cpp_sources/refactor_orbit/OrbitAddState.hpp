//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#ifndef USER_INTERFACE_ORBIT_ADD_MODE_HPP
#define USER_INTERFACE_ORBIT_ADD_MODE_HPP

#include "OrbitState.hpp"

class OrbitAddState : public OrbitState
{
  friend class DirectManipulation;
public:
  OrbitAddState(Orbit* _orbitScreen);

  virtual void enterState();
  virtual void exitState();

  virtual void update(float timeStep);
  
  virtual bool mouseClickEvent(int buttonID, unsigned clickCount, int x, int y);
  virtual void mouseMotionEvent(int x, int y, float deltaX, float deltaY, float deltaWheel);
  virtual bool keyboardEvent(char keyCode, bool pressed);

  void setPrototype(Module* _prototype);
  const Module* getLastPrototype();

  virtual void outputString(wostream& out);
private:
  void updateTargetPort(int x, int y);
  void setTargetPort(Port* targetPort);
  void orientMarker();
  void selectNextConnectionPort();

  Module* currentPrototype;
  const Module* prototypeStack0;
  const Module* prototypeStack1;
  Port* connectionPort;
  Port* targetPort;
  set<pair<Port*, Port*> > connections;  

  bool mTargetPortDirty;
  Degree rotateDegree;
  Entity* addEntity;
  SceneNode* addNodeModule;
  SceneNode* addNodeBox;

  bool mPositionMarkerVisible;
  Matrix mModuleCenterTransform;
  Matrix mPositionMarkerTransform;
};



#endif
