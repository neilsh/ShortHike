//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "CameraLayer.hpp"

#include "rendering/Camera.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderSystem.hpp"


// --------------------------------------------------------------------------------
// Singleton implementation

CameraLayer*
rCameraLayer()
{
  static CameraLayer* mCameraLayer = new CameraLayer();
  return mCameraLayer;
}

// --------------------------------------------------------------------------------

CameraLayer::CameraLayer()
  : mCamera(NULL), mFocusMesh(NULL),
    mMovingUp(false), mMovingDown(false),
    mMovingLeft(false), mMovingRight(false),
    mMovingForward(false), mMovingBackward(false)
{
  mFocusMesh = rRenderSystem()->createMesh();
  mFocusMesh->load("assets/user_interface/focus.shd");
}

// --------------------------------------------------------------------------------

void
CameraLayer::setCamera(Camera* camera)
{
  mCamera = camera;
}

// --------------------------------------------------------------------------------

const float CAMERA_MOVE_SPEED = 25;

void
CameraLayer::update(float timestep)
{
  Vector translation(0, 0, 0);
  if(mMovingLeft) translation.x -= 1;
  if(mMovingRight) translation.x += 1;
  if(mMovingUp) translation.y += 1;
  if(mMovingDown) translation.y -= 1;
  if(mMovingForward) translation.z += 1;
  if(mMovingBackward) translation.z -= 1;
  translation *= CAMERA_MOVE_SPEED * timestep;
  mCamera->translateLocal(translation);
}


void
CameraLayer::render(RenderSystem* renderSystem, int, int)
{
  renderSystem->enableLighting(true);
  renderSystem->setProjectionMatrix(mCamera);
  renderSystem->setViewMatrix(mCamera->getViewMatrix());
  renderSystem->setColor(0, 0, 0.8);
  renderSystem->setMesh(mFocusMesh);
  renderSystem->renderMesh(Matrix(mCamera->getFocus()));
  renderSystem->setColor(1, 1, 1);
}



// --------------------------------------------------------------------------------

bool
CameraLayer::mouseDownEvent(int buttonID, int x, int y)
{
  if(mCamera == NULL) return false;
  if(0 == buttonID) {
    mCamera->startOrbit(Vector(x, y));
  }
  else if(1 == buttonID) {
    mCamera->startZoom(Vector(x, y));
  }  
  return false;
}

bool
CameraLayer::mouseUpEvent(int buttonID, int x, int y)
{
  if(mCamera == NULL) return false;
  mCamera->stop();
  return false;
}

void
CameraLayer::mouseMotionEvent(int x, int y, float deltaX, float deltaY, float deltaWheel)
{
  if(mCamera == NULL) return;
  mCamera->mouseMove(Vector(x, y));
}

bool
CameraLayer::keyboardEvent(char keyCode, bool pressed)
{
  if('A' == keyCode || VK_LEFT == keyCode) {
    mMovingLeft = pressed;
    return true;
  }
  else if('D' == keyCode || VK_RIGHT == keyCode) {
    mMovingRight = pressed;
    return true;
  }
  else if('W' == keyCode || VK_UP == keyCode) {
    mMovingUp = pressed;
    return true;
  }  
  else if('S' == keyCode || VK_DOWN == keyCode) {
    mMovingDown = pressed;
    return true;
  }
  else if('R' == keyCode || VK_PRIOR == keyCode) {
    mMovingForward = pressed;
    return true;
  }
  else if('F' == keyCode || VK_NEXT == keyCode) {
    mMovingBackward = pressed;
    return true;
  }
  
  return false;
}

