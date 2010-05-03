//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef LAYERS_CAMERA_LAYER_HPP
#define LAYERS_CAMERA_LAYER_HPP

#include "guilib/Widget.hpp"

class Camera;
class Mesh;

class CameraLayer : public Widget
{
  friend CameraLayer* rCameraLayer();
public:
  void setCamera(Camera* camera);
  
  virtual bool mouseDownEvent(int buttonID, int x, int y);
  virtual bool mouseUpEvent(int buttonID, int x, int y);
  virtual void mouseMotionEvent(int x, int y, float deltaX, float deltaY, float deltaWheel);
  virtual bool keyboardEvent(char keyCode, bool pressed);

  virtual void update(float timestep);
  virtual void render(RenderSystem* renderSystem, int x, int y);
private:
  CameraLayer();

  Camera* mCamera;
  Mesh* mFocusMesh;

  bool mMovingRight;
  bool mMovingLeft;
  bool mMovingUp;
  bool mMovingDown;
  bool mMovingForward;
  bool mMovingBackward;

};


#endif
