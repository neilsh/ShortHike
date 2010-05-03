//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import RenderSystem;
private import GUIManager;
private import Rect;
private import Font;
private import Texture;
private import CacheManager;
private import ChunkData;


// --------------------------------------------------------------------------------

struct Prop
{
  Mesh mesh;
  Texture texture;
}

private Prop[char[]] mProps;


void
doProp(char[] fileName, Matrix transform, bool alphaBlend = false)
{
  if(fileName in mProps is null) {
    ChunkData chunkData;
    if(!rCacheManager().aquireChunkData(fileName, chunkData)) {
      logError << "Unable to load prop:" << fileName << endl;
      return;
    }
    ChunkIterator curr = chunkData.begin();

    Prop newProp;
    newProp.mesh = rRenderSystem.createMesh(true);
    newProp.texture = rRenderSystem.createTexture();
    bool meshLoaded = false;
    bool textureLoaded = false;
    while(curr != chunkData.end()) {
      if(!meshLoaded && curr.getChunkType() == "MESH") {
        newProp.mesh.load(curr, chunkData.end(), curr);
        meshLoaded = true;
      }
      else if(!textureLoaded && curr.getChunkType() == "IRGB") {
        newProp.texture.load(curr, chunkData.end(), curr);
        textureLoaded = true;
      }
      else {
        curr++;
      }
    }
    mProps[fileName] = newProp;
  }
  Prop prop = mProps[fileName];
  rRenderSystem.enableAlphaBlending(alphaBlend);
  rRenderSystem.setMesh(prop.mesh);
  rRenderSystem.setMaterialTexture(prop.texture);
  rRenderSystem.renderMesh(transform);
}

// --------------------------------------------------------------------------------

bool
doProp(char[] fileName, Matrix transform, Ray pickRay, out float oDistance)
{
  doProp(fileName, transform, false);
  Prop prop = mProps[fileName];
  if(prop.mesh.intersectRayBox(pickRay, transform, oDistance)) {
    if(prop.mesh.intersectRayTri(pickRay, transform, oDistance)) {
      return true;
    }
  }
  return false;
}
