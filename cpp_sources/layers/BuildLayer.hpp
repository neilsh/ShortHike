//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef LAYERS_BUILD_LAYER_HPP
#define LAYERS_BUILD_LAYER_HPP

#include "guilib/Widget.hpp"

class Mesh;
class Texture;
class Port;
class Module;
class Prototype;
class Station;
class Camera;

typedef vector<pair<Module*,unsigned int> > PortConnectionList;

class BuildLayer : public Widget
{
  friend BuildLayer* rBuildLayer();
public:
  BuildLayer();  

  virtual void update(float);
  virtual void render(RenderSystem* renderSystem, int deltaX, int deltaY);
  virtual bool mouseClickEvent(int buttonID, unsigned clickCount, int x, int y);

  void setReadyState();
  void setAddState(Prototype* prototype);
  void setSelectedState(Module* module);
  
  void rotateLeft();
  void rotateRight();
  void switchPort();
  void deleteModule();

  void setCamera(Camera* camera) {mCamera = camera;}
  void setStation(Station* station);
  Station* getStation() {return mStation;}

  Module* getSelectedModule();

  wstring outputString();
private:
  void renderPrototype(RenderSystem* renderSystem);

  enum State {
    STATE_READY,
    STATE_ADD,
    STATE_SELECTED
  };

  State mState;
  Station* mStation;
  Camera* mCamera;
  
  Prototype* mPrototype;
  Module* mSelectedModule;
  float mAngle;
  unsigned int mPort;

  Mesh* mPortMesh;
  Texture* mPortTexture;  
  Mesh* mConnectionMesh;
  Texture* mConnectionTexture;  
  Mesh* mSelectedMesh;
  Texture* mSelectedTexture;

  Mesh* mFocusMesh;
  Texture* mFocusTexture;

  enum MouseOverItem {
    MOUSEOVER_BACKGROUND,
    MOUSEOVER_MODULE,
    MOUSEOVER_PORT
  };
  
  MouseOverItem mMouseOver;
  Module* mMouseOverModule;
  int mMouseOverPortIndex;  
};


#endif
