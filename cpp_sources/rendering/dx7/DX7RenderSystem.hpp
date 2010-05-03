//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_DX7_DX7RENDERSYSTEM_HPP
#define RENDERING_DX7_DX7RENDERSYSTEM_HPP

#include "rendering/RenderSystem.hpp"

class DX7Mesh;

class DX7RenderSystem : public RenderSystem
{
public:
  DX7RenderSystem();
  virtual ~DX7RenderSystem();
  
  bool initialize();
  bool initializeDevice(HWND hWindow, const GUID* deviceGUID);
  IDirect3D7* getDirect3D() {return mDirect3D;}
  IDirect3DDevice7* getDirect3DDevice() {return mDirect3DDevice;}
  IDirectDraw7* getDirectDraw() {return mDirectDraw;}

  bool isHardwareAccelerated() const {return true;}
  DDPIXELFORMAT getRGBAFormat();

  // ----------------------------------------
  // RenderSystem implementation

  virtual Mesh* createMesh(bool collisionSupport);
  virtual void destroyMesh(Mesh*);
  virtual Texture* createTexture();
  virtual void destroyTexture(Texture* texture);
  virtual Surface* createSurface(int width, int height);
  virtual void destroySurface(Surface*);
  virtual Camera* createCamera();
  virtual void destroyCamera(Camera*);

  virtual int getWidth();
  virtual int getHeight();

  virtual bool beginScene();
  virtual void endScene();

  virtual void renderSurface(Surface* surface, int xpos, int ypos);  

  // Lighting
  virtual void setSunLight(Vector, Color);
  virtual void setPlanetLight(Vector, Color);
  virtual void enableLighting(bool);
  virtual void enableSunLight(bool);
  virtual void enablePlanetLight(bool);
  virtual bool getCapsLighting();

  // Material support
  virtual void enableVertexColor(bool);
  virtual bool getCapsVertexColor();

  virtual void setColor(float r, float g, float b);

  virtual void setColorTexture(Texture* texture);
  virtual void setColorTextureFunction(ColorTextureFunction);
  virtual void enableColorTexture(bool);
  virtual bool getCapsColorTexture();
  virtual bool getCapsSpecularFunction();
  
  virtual void setStructureTexture(Texture* texture);
  virtual void enableStructureTexture(bool);
  virtual bool getCapsStructureTexture();

  virtual void setDecalTexture(Texture* texture);
  virtual void enableDecalTexture(bool);
  virtual bool getCapsDecalTexture();

  virtual void enableZBuffer(bool);
  virtual void enableAlphaBlending(bool);
  virtual bool getCapsAlphaBlending();
  
  virtual void setProjectionMatrix(Camera*);
  virtual void setViewMatrix(const Matrix& viewMatrix);
  virtual void setMesh(Mesh*, bool wireframe, bool cull);
  virtual void renderMesh(const Matrix&);

  virtual int getFrameTriangles() {return mFrameTriangles;}  
private:
  void cleanUpDevice();

  // These have lifetimes with the class
  IDirectDraw7* mDirectDraw;
  IDirect3D7* mDirect3D;

  // These can be renewed in case of fullscreen/windowed switch
  const GUID* mDeviceGUID;
  IDirectDrawSurface7* mPrimarySurface;
  IDirectDrawSurface7* mBackBuffer;
  IDirectDrawSurface7* mZBuffer;
  IDirect3DDevice7* mDirect3DDevice;
  RECT mScreenRect;

  DX7Mesh* mCurrentMesh;

  int mFrameTriangles;
};

#endif
