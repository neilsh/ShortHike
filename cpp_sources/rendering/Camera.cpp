//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "Camera.hpp"

//--------------------------------------------------------------------------------

const float CAMERA_MINIMUM_DISTANCE = 5.0;
const float CAMERA_MAXIMUM_DISTANCE = 8000.0;
const float CAMERA_INITIAL_DISTANCE = 75.0;

// const float TRACKBALL_SIZE = 0.7f;
// const float TRACKBALL_ROTATE_ACCELERATE = 1.8f;
// const float TRACKBALL_ZOOM_ACCELERATE = 0.2f;

const float ROTATE_ACCELERATION = 0.75f;
const float ZOOM_ACCELERATION = 0.2f;

//--------------------------------------------------------------------------------
// Utility functions

// namespace 
// {

//   Vector
//   projectToSphere(float radius, const Vector& position)
//   {
//     float distance = position.length();
//     // Inside sphere
//     if(distance < radius * 0.70710678118654752440) {
//       return Vector(position[0], -position[1], Math::Sqrt(radius * radius - distance * distance));
//     }
//     // Hyperbola
//     else {
//       float temp = radius / 1.41421356237309504880;
//       return Vector(position[0], -position[1], temp * temp / distance);
//     }  
//   }



//   Quaternion
//   trackBall(const Vector& mousePos1, const Vector& mousePos2)
//   {
//     if(mousePos1 == mousePos2) {
//       return Quaternion::IDENTITY;
//     }
  
//     Vector position1 = projectToSphere(TRACKBALL_SIZE, mousePos1);
//     Vector position2 = projectToSphere(TRACKBALL_SIZE, mousePos2);
  
//     Vector axis = position1.crossProduct(position2);
//     axis.normalise();
//     Real temp = (position1 - position2).length() / (2.0f * TRACKBALL_SIZE);

//     if(temp > 1.0) temp = 1.0;
//     if(temp < -1.0) temp = -1.0;
//     Real phi = 2.0 * asin(temp) * TRACKBALL_ROTATE_ACCELERATE;
  
//     return Quaternion(-Radian(phi), axis);
//   }
// }


//--------------------------------------------------------------------------------

Camera::Camera()
  : mState(READY), mLastMousePosition(-1, -1), mYaw(0), mPitch(0), mDistance(CAMERA_INITIAL_DISTANCE), mFocus(Vector::ZERO),
    mOrientation(Quaternion::IDENTITY), mPosition(Vector::ZERO), mTransform(Matrix::ZERO),
    mFieldOfViewY(Math::PI/4.0), mNearClipDistance(1.0f), mFarClipDistance(1000.0f), mAspectRatio(1.33333333333333f)
{
  update();
}

Camera::~Camera()
{
}

//--------------------------------------------------------------------------------
// Accessors

void
Camera::setFocus(const Vector& focusPosition)
{
  mFocus = focusPosition;
  mDistance = CAMERA_INITIAL_DISTANCE;
  update();
}


//--------------------------------------------------------------------------------
// Start the different modes

void
Camera::startOrbit(const Vector& mousePosition)
{
  stop();
  mState = ORBITING;
  mLastMousePosition = mousePosition;
}

void
Camera::startZoom(const Vector& mousePosition)
{
  stop();
  mState = ZOOMING;
  mLastMousePosition = mousePosition;
}

void
Camera::startMove(const Vector& mousePosition)
{
  stop();
  mState = MOVING;
  mLastMousePosition = mousePosition;
}

//--------------------------------------------------------------------------------

void
Camera::mouseMove(const Vector& currentPosition)
{
  if(mState == ORBITING) {
//     newOrientation = trackBall(startPosition, currentPosition);
    mYaw += -(mLastMousePosition.x - currentPosition.x) * ROTATE_ACCELERATION;
    mPitch += -(mLastMousePosition.y - currentPosition.y) * ROTATE_ACCELERATION;
  }
  else if(mState == ZOOMING) {
    mDistance += -(mLastMousePosition.y - currentPosition.y) * ZOOM_ACCELERATION;
  }
  else if(mState == MOVING) {
  }  
  mLastMousePosition = currentPosition;
  update();
}

//--------------------------------------------------------------------------------

void
Camera::stop()
{
  mState = READY;
}

//--------------------------------------------------------------------------------

void
Camera::update()
{
  mPitch = clamp(mPitch, -85.0f, 85.0f);
  while(mYaw > 360)
    mYaw -= 360;
  while(mYaw < 0)
    mYaw += 360;
  mDistance = clamp(mDistance, CAMERA_MINIMUM_DISTANCE, CAMERA_MAXIMUM_DISTANCE);
  
  Matrix distanceMatrix(Vector(0, 0, -mDistance));
  Matrix pitchMatrix = Matrix(Quaternion(Vector(1, 0, 0), Degree(mPitch)));
  Matrix yawMatrix = Matrix(Quaternion(Vector(0, 1, 0), Degree(mYaw)));
  
  Matrix focusMatrix(mFocus);
  
  mTransform = distanceMatrix * pitchMatrix * yawMatrix * focusMatrix;
  mPosition = mTransform.getPosition();
  mOrientation = mTransform.getOrientation();
}

//--------------------------------------------------------------------------------

Vector
Camera::getPosition() const
{
  return mPosition;
}

Quaternion
Camera::getOrientation() const
{
  return mOrientation;
}



Matrix
Camera::getViewMatrix() const
{  
  Matrix viewMatrix = mTransform.getRotationMatrix();
  viewMatrix.transposeRotation();
  Vector translation = mPosition * viewMatrix;
  translation *= -1;
  viewMatrix.setTranslation(translation);
  return viewMatrix;
}

//--------------------------------------------------------------------------------

void
Camera::translateLocal(Vector translation)
{
  Vector globalTranslation = translation * mOrientation;
  mFocus += globalTranslation;
  update();
}


//--------------------------------------------------------------------------------

Ray
Camera::getCameraToViewportRay(Real screenX, Real screenY) const
{
  Real centeredScreenX = (screenX - 0.5f);
  Real centeredScreenY = (0.5f - screenY);
  
  Real normalizedSlope = tanf(mFieldOfViewY / 2);
  Real viewportYToWorldY = normalizedSlope * mNearClipDistance * 2;
  Real viewportXToWorldX = viewportYToWorldY * mAspectRatio;
  
  Vector rayOrigin = getPosition();

  Vector rayDirection;
  rayDirection.x = centeredScreenX * viewportXToWorldX;
  rayDirection.y = centeredScreenY * viewportYToWorldY;
  rayDirection.z = mNearClipDistance;
  rayDirection = rayDirection * getOrientation();
  rayDirection.normalize();
  
  return Ray(rayOrigin, rayDirection);
} 


