//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Main;
private import RenderSystem;
private import Mesh;
private import CacheManager;
private import ChunkData;
private import CameraControl;
private import Camera;
private import Rect;
private import GUIManager;
private import Button;
private import Font;
private import Texture;
private import Platform;
private import ChunkStream;

// --------------------------------------------------------------------------------

private Camera mCamera;
private Mesh mMesh;
private Texture mTexture;

void
initToolScreen(RenderSystem renderSystem)
{
  if(mCamera is null)
    mCamera = renderSystem.createCamera();
  if(mMesh is null) {
    mMesh = renderSystem.createMesh(true);
    mMesh.loadCube(10);
  }
  if(mTexture is null) {
    mTexture = renderSystem.createTexture();
    mTexture.loadCheckerBoard();
  }
}


// --------------------------------------------------------------------------------

Screen doToolScreen(float timeStep, RenderSystem renderSystem)
{
  initToolScreen(renderSystem);
  Rect screenRect = new Rect(0, 0, renderSystem.getWidth(), renderSystem.getHeight());
  doCameraControl(timeStep, renderSystem, cID(&doToolScreen, 0), mCamera, screenRect);  
  renderSystem.setView(mCamera.getProjectionMatrix(), mCamera.getViewMatrix());
  renderSystem.setMaterialTexture(mTexture);
  renderSystem.setMesh(mMesh);
  renderSystem.renderMesh(Matrix.IDENTITY);
  doText(cID(&doToolScreen, 100), "Foo!", 15, Color.WHITE, renderSystem.getWidth() - 150, renderSystem.getHeight() - 32);        
  return doButtons(timeStep, renderSystem);
}

// --------------------------------------------------------------------------------


Screen doButtons(float timeStep, RenderSystem renderSystem) {
  // This is the mesh/texture tool
  if(doMenuButton(cID(&doButtons, 100), "Clear", 0)) {
    mMesh.loadCube(10);
    mTexture.loadCheckerBoard();
  }
  if(doMenuButton(cID(&doButtons, 101), "Load SHD", 2)) {
    char[] fileName;
    if(rPlatform().openFileDialog(fileName, "shd")) {
      ChunkData chunkData = new ChunkData(fileName);
      ChunkIterator curr = chunkData.begin();
      bool meshLoaded = false;
      bool textureLoaded = false;
      while(curr != chunkData.end()) {
        if(!meshLoaded && curr.getChunkType() == "MESH") {
          mMesh.load(curr, chunkData.end(), curr);
          meshLoaded = true;
        }
        else if(!textureLoaded && curr.getChunkType() == "IRGB") {
          mTexture.load(curr, chunkData.end(), curr);
          textureLoaded = true;
        }
        else {
          curr++;
        }        
      }
    }    
  }
  if(doMenuButton(cID(&doButtons, 102), "Save SHD", 3)) {
    char[] fileName;
    if(rPlatform().saveFileDialog(fileName, "shd")) {
      ChunkStream cStream = new ChunkStream(fileName);
      mMesh.save(cStream);
      mTexture.save(cStream);
      cStream.close();
    }
  }
  if(doMenuButton(cID(&doButtons, 103), "Import OBJ", 5)) {
    char[] fileName;
    if(rPlatform().openFileDialog(fileName, "obj")) {
      mMesh.loadFromOBJ(fileName);    
    }
  }    
  if(doMenuButton(cID(&doButtons, 104), "Import PNG", 6)){
    char[] fileName;
    if(rPlatform().openFileDialog(fileName, "png")) {
      mTexture.loadFromPNG(fileName);    
    }
  }

  if(doMenuButton(cID(&doButtons, 1), "Shell", 10)) {
    return Screen.SHELL;
  }  

  return Screen.TOOLS;
}
