//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef RENDERING_CAMERA
#define RENDERING_CAMERA


class Camera
{
  friend class DX7RenderSystem;
public:
  ~Camera();

  enum State { ORBITING, ZOOMING, MOVING, READY };  
  State getState() {return mState;}

  void startOrbit(const Vector& mousePosition);
  void startZoom(const Vector& mousePosition);
  void startMove(const Vector& mousePosition);

  void mouseMove(const Vector& mousePosition);
  void stop();

  void setFocus(const Vector& focusPosition);
  const Vector& getFocus() const {return mFocus;}

  Vector getPosition() const;
  Quaternion getOrientation() const;
  Matrix getViewMatrix() const;

  void translateLocal(Vector translation);

  Ray getCameraToViewportRay(float screenX, float screenY) const;
private:
  Camera();
  void update();

  State mState;
  Vector mLastMousePosition;

  float mFieldOfViewY;
  float mAspectRatio;
  float mNearClipDistance;
  float mFarClipDistance;

  Real mYaw;
  Real mPitch;
  Real mDistance;  
  Vector mFocus;

  Quaternion mOrientation;
  Vector mPosition;
  Matrix mTransform;
};


#endif
