//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;

private import Camera;
private import Mesh;
private import Platform;
private import Texture;
private import Rect;

// FIXME: This should be a proper type
struct Color
{
  float r;
  float g;
  float b;
  float a;
  static const Color WHITE = {r:1, g:1, b:1, a:1};
  static const Color BLACK = {r:0, g:0, b:0, a:1};
}


Color cColor(float _r = 0, float _g = 0, float _b = 0, float _a = 1.0)
{
  Color c;
  c.r = _r;
  c.g = _g;
  c.b = _b;
  c.a = _a;
  return c;
}


struct LightEnv
{
  Vector primaryDirection;
  Color primaryColor;
  Vector secondaryDirection;
  Color secondaryColor;
}


// Meshes and textures are just raw data areas.
// 
class RenderSystem
{
public:
  ~this() {}

  abstract Mesh createMesh(bool collisionSupport = false);
  abstract Texture createTexture();
  abstract Camera createCamera();

  abstract int getWidth();
  abstract int getHeight();

  abstract bool beginScene();
  abstract void endScene();

  // Lighting
  abstract void enableLighting(LightEnv lightEnv);
  abstract void disableLighting();

  // Material support
  abstract void setMaterialColor(Color diffuse, Color specular, Color ambient);
  abstract void setMaterialTexture(Texture texture);

  abstract void enableZBuffer(bool);
  abstract void enableAlphaBlending(bool);
  abstract void enableSpecular(bool);
  
  abstract void setView(Matrix projectionMatrix, Matrix viewMatrix);
  abstract void setViewScreen();

  abstract void setMesh(Mesh, bool wireframe = false, bool cull = true);
  abstract void renderMesh(Matrix modelMatrix);
  abstract void renderTexture(Texture texture, Rect screenRect, Rect textureRect, Matrix transform = Matrix.IDENTITY);
  
  //----------------------------------------
  // Optional stat functions
  abstract int getFrameTriangles() {return 0;}
};

private RenderSystem mRenderSystem = null;

RenderSystem
rRenderSystem()
{
  if(mRenderSystem is null)
    mRenderSystem = rPlatform().createRenderSystem();
  return mRenderSystem;
}
