//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef USER_INTERFACE_TRACKBALLCAMERA
#define USER_INTERFACE_TRACKBALLCAMERA


class TrackBallCamera
{
public:
  TrackBallCamera(SceneManager* sceneManager);
  ~TrackBallCamera();

  enum State { ORBITING, ZOOMING, MOVING, READY };  
  State getState() {return state;}

  void startOrbit(const Vector2& mousePosition);
  void startZoom(const Vector2& mousePosition);
  void startMove(const Vector2& mousePosition);

  void mouseMove(const Vector2& mousePosition);
  void stop(const Vector2& mousePosition);

  void setFocus(const Vector& focusPosition);

  Vector getPosition() const;
  Quaternion getOrientation() const;
  Matrix4 getViewMatrix() const;

  Camera* getCamera() {return camera;}
  SceneNode* getFocusNode() {return focusNode;}
  SceneNode* getCameraNode() {return cameraNode;}

  Ray getCameraToViewportRay(Real screenX, Real screenY) const;

  float mFieldOfViewY;
  float mAspectRatio;
  float mNearClipDistance;
  float mFarClipDistance;
private:
  void update();

  State state;

  Vector2 startPosition;

  Quaternion currentOrientation;
  Quaternion newOrientation;

  Real mCurrentYaw;
  Real mNewYaw;
  Real mCurrentPitch;
  Real mNewPitch;
  
  Real currentDistance;
  Real newDistance;
  
  Vector currentPosition;
  Vector newPosition;

  Matrix4 mTransform;

  Camera* camera;
  SceneNode* focusNode;
  SceneNode* cameraNode;
};


#endif
