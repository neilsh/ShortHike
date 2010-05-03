//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Point;
private import RenderSystem;

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

// --------------------------------------------------------------------------------


class Camera
{
  enum State { ORBITING, ZOOMING, MOVING, READY };  
  State getState() {return mState;}

  //   void startOrbit(const Vector& mousePosition);
  //   void startZoom(const Vector& mousePosition);
  //   void startMove(const Vector& mousePosition);

  //   void mouseMove(const Vector& mousePosition);
  //   void stop();

  //   void setFocus(const Vector& focusPosition);

  //   Vector getPosition() const;
  //   Quaternion getOrientation() const;
  //   Matrix getViewMatrix() const;

  //   void translateLocal(Vector translation);

  //   Ray getCameraToViewportRay(float screenX, float screenY) const;




  //--------------------------------------------------------------------------------

  this()
  {
    mState = State.READY;
    mDistance = CAMERA_INITIAL_DISTANCE;
    mOrientation = Quaternion.IDENTITY;
    mPosition = Vector.ZERO;
    mTransform = Matrix.IDENTITY;

    mYaw = 0;
    mPitch = 0;
    mFocus = Vector.ZERO;

    mFieldOfViewY = PI/4.0;
    mNearClipDistance = 1;
    mFarClipDistance = 10000;
    mAspectRatio = 1.33333333333f;
    update();
  }

  ~this()
  {
  }

  //--------------------------------------------------------------------------------
  // Accessors

  Vector getFocus() {return mFocus;}

  void
  setFocus(Vector focusPosition)
  {
    mFocus = focusPosition;
    mDistance = CAMERA_INITIAL_DISTANCE;
    update();
  }


  //--------------------------------------------------------------------------------
  // Start the different modes

  void
  startOrbit(Vector mousePosition)
  {
    stop();
    mState = State.ORBITING;
    mLastMousePosition = mousePosition;
  }

  void
  startZoom(Vector mousePosition)
  {
    stop();
    mState = State.ZOOMING;
    mLastMousePosition = mousePosition;
  }

  void
  startMove(Vector mousePosition)
  {
    stop();
    mState = State.MOVING;
    mLastMousePosition = mousePosition;
  }

  //--------------------------------------------------------------------------------

  void
  mouseMove(Vector currentPosition)
  {
    if(mState == State.ORBITING) {
      //     newOrientation = trackBall(startPosition, currentPosition);
      mYaw += -(mLastMousePosition.x - currentPosition.x) * ROTATE_ACCELERATION;
      mPitch += -(mLastMousePosition.y - currentPosition.y) * ROTATE_ACCELERATION;
    }
    else if(mState == State.ZOOMING) {
      mDistance += -(mLastMousePosition.y - currentPosition.y) * ZOOM_ACCELERATION;
    }
    else if(mState == State.MOVING) {
    }  
    mLastMousePosition = currentPosition;
    update();
  }

  //--------------------------------------------------------------------------------

  void
  stop()
  {
    mState = State.READY;
  }

  //--------------------------------------------------------------------------------

  void
  update()
  {
    mPitch = clamp(mPitch, -85.0f, 85.0f);
    while(mYaw > 360)
      mYaw -= 360;
    while(mYaw < 0)
      mYaw += 360;
    mDistance = clamp(mDistance, CAMERA_MINIMUM_DISTANCE, CAMERA_MAXIMUM_DISTANCE);
  
    Matrix distanceMatrix = createMatrix(createVector(0, 0, -mDistance));
    Matrix pitchMatrix = createMatrix(createQuaternion(createVector(1, 0, 0), radFromDeg(mPitch)));
    Matrix yawMatrix = createMatrix(createQuaternion(createVector(0, 1, 0), radFromDeg(mYaw)));
  
    Matrix focusMatrix = createMatrix(mFocus);

    mTransform = distanceMatrix * pitchMatrix * yawMatrix * focusMatrix;
    mPosition = mTransform.getPosition();
    mOrientation = mTransform.getOrientation();
  }

  //--------------------------------------------------------------------------------

  Vector
  getPosition()
  {
    return mPosition;
  }

  Quaternion
  getOrientation()
  {
    return mOrientation;
  }


  Matrix
  getProjectionMatrix()
  {
    float theta = mFieldOfViewY * 0.5;
    float heightScale = 1 / tan(theta);
    float widthScale = heightScale / mAspectRatio;

    float depthScale = mFarClipDistance / ( mFarClipDistance - mNearClipDistance );
    float depthShift = -depthScale * mNearClipDistance;

    Matrix projectionMatrix = Matrix.ZERO;
    //   projectionMatrix[0][0] = widthScale;
    //   projectionMatrix[1][1] = heightScale;  
    //   projectionMatrix[2][2] = depthScale;
    //   projectionMatrix[2][3] = 1.0f;
    //   projectionMatrix[3][2] = depthShift;

    projectionMatrix._11 = widthScale;
    projectionMatrix._22 = heightScale;
    projectionMatrix._33 = depthScale;
    projectionMatrix._34 = 1.0f;
    projectionMatrix._43 = depthShift;
  
    return projectionMatrix;
  }


  Matrix
  getViewMatrix()
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
  translateLocal(Vector translation)
  {
    Vector globalTranslation = translation * mOrientation;
    mFocus += globalTranslation;
    update();
  }


  //--------------------------------------------------------------------------------

  Ray
  getCameraToViewportRay(float screenX, float screenY)
  {
    float centeredScreenX = (screenX - 0.5f);
    float centeredScreenY = (0.5f - screenY);
  
    float normalizedSlope = tan(mFieldOfViewY / 2);
    float viewportYToWorldY = normalizedSlope * mNearClipDistance * 2;
    float viewportXToWorldX = viewportYToWorldY * mAspectRatio;
  
    Vector rayOrigin = getPosition();

    Vector rayDirection;
    rayDirection.x = centeredScreenX * viewportXToWorldX;
    rayDirection.y = centeredScreenY * viewportYToWorldY;
    rayDirection.z = mNearClipDistance;
    rayDirection = rayDirection * getOrientation();
    rayDirection.normalize();
  
    return cRay(rayOrigin, rayDirection);
  } 

  // --------------------------------------------------------------------------------

  Point
  getScreenPosition(Vector worldPosition)
  {
    return new Point(cast(int)(rRenderSystem.getWidth() / 2), cast(int)(rRenderSystem.getHeight() / 2));
  }

private:
  State mState;
  Vector mLastMousePosition;

  float mFieldOfViewY;
  float mAspectRatio;
  float mNearClipDistance;
  float mFarClipDistance;

  float mYaw;
  float mPitch;
  float mDistance;  
  Vector mFocus;

  Quaternion mOrientation;
  Vector mPosition;
  Matrix mTransform;
}


