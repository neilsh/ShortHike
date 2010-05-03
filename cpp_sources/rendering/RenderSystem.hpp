//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_RENDERSYSTEM_HPP
#define RENDERING_RENDERSYSTEM_HPP

#include "math/Matrix.hpp"
#include "math/Vector.hpp"

class Camera;
class Mesh;
class Texture;
class Surface;

// FIXME: This should be a proper type
typedef float Color;

// Meshes and textures are just raw data areas.
// 
class RenderSystem
{
public:
  ~RenderSystem() {}

  enum ColorTextureFunction {
    TRANSPARENCY,
    SPECULAR
  };

  virtual Mesh* createMesh(bool collisionSupport = false) = 0;
  virtual void destroyMesh(Mesh*) = 0;
  virtual Texture* createTexture() = 0;
  virtual void destroyTexture(Texture* texture) = 0;
  virtual Surface* createSurface(int width = 0, int height = 0) = 0;
  virtual void destroySurface(Surface*) = 0;
  virtual Camera* createCamera() = 0;
  virtual void destroyCamera(Camera*) = 0;

  virtual int getWidth() = 0;
  virtual int getHeight() = 0;

  virtual bool beginScene() = 0;
  virtual void endScene() = 0;

  virtual void renderSurface(Surface*, int xpos, int ypos) = 0;

  // Lighting
  virtual void setSunLight(Vector, Color) = 0;
  virtual void setPlanetLight(Vector, Color) = 0;
  virtual void enableLighting(bool) = 0;
  virtual void enableSunLight(bool) = 0;
  virtual void enablePlanetLight(bool) = 0;
  virtual bool getCapsLighting() = 0;

  // Material support
  virtual void enableVertexColor(bool) = 0;
  virtual bool getCapsVertexColor() = 0;

  virtual void setColor(float r, float g, float b) = 0;

  virtual void setColorTexture(Texture* texture) = 0;
  virtual void setColorTextureFunction(ColorTextureFunction) = 0;
  virtual void enableColorTexture(bool) = 0;
  virtual bool getCapsColorTexture() = 0;
  
  virtual void setStructureTexture(Texture* texture) = 0;
  virtual void enableStructureTexture(bool) = 0;
  virtual bool getCapsStructureTexture() = 0;

  virtual void setDecalTexture(Texture* texture) = 0;
  virtual void enableDecalTexture(bool) = 0;
  virtual bool getCapsDecalTexture() = 0;

  virtual void enableZBuffer(bool) = 0;
  virtual void enableAlphaBlending(bool) = 0;
  virtual bool getCapsAlphaBlending() = 0;
  
  virtual void setProjectionMatrix(Camera*) = 0;
  virtual void setViewMatrix(const Matrix& viewMatrix) = 0;
  virtual void setMesh(Mesh*, bool wireframe = false, bool cull = true) = 0;
  virtual void renderMesh(const Matrix& modelMatrix) = 0;
  //----------------------------------------
  // Optional stat functions
  virtual int getFrameTriangles() {return 0;}
};

bool createDX7RenderSystem(HWND hWnd, RenderSystem** outRenderSystem);


#endif
