//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "TrackBallCamera.hpp"

//--------------------------------------------------------------------------------

const float CAMERA_MINIMUM_DISTANCE = 20.0;
const float CAMERA_MAXIMUM_DISTANCE = 8000.0;
const float CAMERA_INITIAL_DISTANCE = 75.0;

const float TRACKBALL_SIZE = 0.7f;
const float TRACKBALL_ROTATE_ACCELERATE = 1.8f;
const float TRACKBALL_ZOOM_ACCELERATE = 250.0f;

const float ROTATE_ACCELERATION = 150;

//--------------------------------------------------------------------------------
// Utility functions


Vector
projectToSphere(float radius, const Vector2& position)
{
  float distance = position.length();
  // Inside sphere
  if(distance < radius * 0.70710678118654752440) {
    return Vector(position[0], -position[1], Math::Sqrt(radius * radius - distance * distance));
  }
  // Hyperbola
  else {
    float temp = radius / 1.41421356237309504880;
    return Vector(position[0], -position[1], temp * temp / distance);
  }  
}



Quaternion
trackBall(const Vector2& mousePos1, const Vector2& mousePos2)
{
  if(mousePos1 == mousePos2) {
    return Quaternion::IDENTITY;
  }
  
  Vector position1 = projectToSphere(TRACKBALL_SIZE, mousePos1);
  Vector position2 = projectToSphere(TRACKBALL_SIZE, mousePos2);
  
  Vector axis = position1.crossProduct(position2);
  axis.normalise();
  Real temp = (position1 - position2).length() / (2.0f * TRACKBALL_SIZE);

  if(temp > 1.0) temp = 1.0;
  if(temp < -1.0) temp = -1.0;
  Real phi = 2.0 * asin(temp) * TRACKBALL_ROTATE_ACCELERATE;
  
  return Quaternion(-Radian(phi), axis);
}


//--------------------------------------------------------------------------------

TrackBallCamera::TrackBallCamera(SceneManager* sceneManager)
  : state(READY), currentPosition(0, 0, 0), newPosition(0, 0, 0),
    currentDistance(CAMERA_INITIAL_DISTANCE), newDistance(0),
    currentOrientation(Quaternion::IDENTITY), newOrientation(Quaternion::IDENTITY),
    mCurrentYaw(0), mNewYaw(0), mCurrentPitch(0), mNewPitch(0)
{
  camera = sceneManager->createCamera("UserCamera");
  camera->setNearClipDistance(5);
  camera->setLodBias(0.5);
  
  focusNode = sceneManager->createSceneNode("TrackBallCameraFocus");
  cameraNode = sceneManager->createSceneNode("TrackBallCameraSelf");
  cameraNode->attachObject(camera);
  sceneManager->getRootSceneNode()->addChild(focusNode);
  focusNode->addChild(cameraNode);

  update();

  mFieldOfViewY = Math::PI/4.0;
  mNearClipDistance = 1.0f;
  mFarClipDistance = 5000.0f;
  mAspectRatio = 1.33333333333333f;
}

TrackBallCamera::~TrackBallCamera()
{
}

//--------------------------------------------------------------------------------
// Accessors

void
TrackBallCamera::setFocus(const Vector& focusPosition)
{
  currentPosition = focusPosition;
  currentDistance = CAMERA_INITIAL_DISTANCE;
  update();
}


//--------------------------------------------------------------------------------
// Start the different modes

void
TrackBallCamera::startOrbit(const Vector2& mousePosition)
{
  stop(mousePosition);
  state = ORBITING;
  startPosition = mousePosition;
}

void
TrackBallCamera::startZoom(const Vector2& mousePosition)
{
  stop(mousePosition);
  state = ZOOMING;
  startPosition = mousePosition;
}

void
TrackBallCamera::startMove(const Vector2& mousePosition)
{
  stop(mousePosition);
  state = MOVING;
  startPosition = mousePosition;
}

//--------------------------------------------------------------------------------

void
TrackBallCamera::mouseMove(const Vector2& currentPosition)
{
  if(state == ORBITING) {
    newOrientation = trackBall(startPosition, currentPosition);
    mNewYaw = (startPosition.x - currentPosition.x) * ROTATE_ACCELERATION;
    mNewPitch = (startPosition.y - currentPosition.y) * ROTATE_ACCELERATION;
  }
  else if(state == ZOOMING) {
    newDistance = (currentPosition[1] - startPosition[1]) * TRACKBALL_ZOOM_ACCELERATE;
  }
  else if(state == MOVING) {
  }  
  update();
}

//--------------------------------------------------------------------------------

void
TrackBallCamera::stop(const Vector2& mousePosition)
{
  mouseMove(mousePosition);

  currentOrientation = currentOrientation * newOrientation;
  currentDistance = currentDistance + newDistance;
  currentPosition = currentPosition + newPosition;

  mCurrentYaw = mCurrentYaw + mNewYaw;
  mCurrentPitch = mCurrentPitch + mNewPitch;
  mNewYaw = 0;
  mNewPitch = 0;
  mCurrentPitch = clamp(mCurrentPitch, -85.0f, 85.0f);

  currentDistance = clamp(currentDistance, CAMERA_MINIMUM_DISTANCE, CAMERA_MAXIMUM_DISTANCE);
  
  newOrientation = Quaternion::IDENTITY;
  newDistance = 0;
  newPosition = Vector::ZERO;

  update();
  
  state = READY;
}

//--------------------------------------------------------------------------------

void
TrackBallCamera::update()
{
  Quaternion tempOrientation = currentOrientation * newOrientation;
  focusNode->setOrientation(tempOrientation);
  
  Real tempDistance = currentDistance + newDistance;
  tempDistance = clamp(tempDistance, CAMERA_MINIMUM_DISTANCE, CAMERA_MAXIMUM_DISTANCE);
  cameraNode->setPosition(0, 0, tempDistance);
  
  Vector tempPosition = currentPosition + newPosition;
  focusNode->setPosition(tempPosition);

  float tempYaw = mCurrentYaw + mNewYaw;
  float tempPitch = mCurrentPitch + mNewPitch;
  tempPitch = clamp(tempPitch, -85.0f, 85.0f);

  Matrix4 distanceMatrix;
  distanceMatrix.makeTrans(0, 0, tempDistance);
  Matrix4 rotateMatrix =   Matrix4(Quaternion(Degree(tempPitch), Vector(1, 0, 0))) * 
    Matrix4(Quaternion(Degree(tempYaw), Vector(0, 1, 0)));
  Matrix4 positionMatrix;
  positionMatrix.makeTrans(tempPosition);
  mTransform = positionMatrix * rotateMatrix * distanceMatrix;
}

//--------------------------------------------------------------------------------

Vector
TrackBallCamera::getPosition() const
{
  return mTransform.getTrans();
}

Quaternion
TrackBallCamera::getOrientation() const
{
  return mTransform.extractQuaternion();
}



Matrix4
TrackBallCamera::getViewMatrix() const
{  
  // ----------------------
  // Update the view matrix
  // ----------------------
  
  // View matrix is:
  //
  //  [ Lx  Uy  Dz  Tx  ]
  //  [ Lx  Uy  Dz  Ty  ]
  //  [ Lx  Uy  Dz  Tz  ]
  //  [ 0   0   0   1   ]
  //
  // Where T = -(Transposed(Rot) * Pos)
  
  // This is most efficiently done using 3x3 Matrices
  
  // Get orientation from quaternion
  
  const Vector& position = getPosition();
  Matrix3 rot;
  mTransform.extract3x3Matrix(rot);

  // Make the translation relative to new axes
  Matrix3 rotT = rot.Transpose();
  Vector trans = -rotT * position;

  // Make final matrix
  Matrix4 viewMatrix = Matrix4::IDENTITY;
  viewMatrix = rotT; // fills upper 3x3
  viewMatrix[0][3] = trans.x;
  viewMatrix[1][3] = trans.y;
  viewMatrix[2][3] = trans.z;

  return viewMatrix;
}


//--------------------------------------------------------------------------------

Ray
TrackBallCamera::getCameraToViewportRay(Real screenX, Real screenY) const
{
  Real centeredScreenX = (screenX - 0.5f);
  Real centeredScreenY = (0.5f - screenY);
  
  Real normalizedSlope = Math::Tan(mFieldOfViewY / 2);
  Real viewportYToWorldY = normalizedSlope * mNearClipDistance * 2;
  Real viewportXToWorldX = viewportYToWorldY * mAspectRatio;
  
  Vector rayOrigin = getPosition();

  Vector rayDirection;
  rayDirection.x = centeredScreenX * viewportXToWorldX;
  rayDirection.y = centeredScreenY * viewportYToWorldY;
  rayDirection.z = -mNearClipDistance;
  rayDirection = getOrientation() * rayDirection;
  rayDirection.normalise();
  
  return Ray(rayOrigin, rayDirection);
} 


