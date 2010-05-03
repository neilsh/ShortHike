//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Location;
private import Camera;
private import RenderSystem;
private import Mesh;
private import Texture;
private import Rect;

// --------------------------------------------------------------------------------
// FIXME: Proper clipping

void doSkyLayer(RenderSystem renderSystem, Location location, Rect clip)
{
  struct PatchInstance
  {
    Mesh mesh;
    Texture groundTexture;
    Texture cloudTexture;
    float distance;
  }

  static const float PLANET_ANGLE = 0.8f;
  
  renderSystem.enableAlphaBlending(true);
  renderSystem.disableLighting();
  renderSystem.enableZBuffer(false);
  
  // FIXME: Move this up into the location generator
  // Some cards, notably GF4000 have issues when these triangles intersect
  // the viewing plane. Thus push everything out.
  Matrix WORKAROUND_scaleMatrix = createMatrix(10);

  // ----------------------------------------
  // Render Sky Objects

  SkyObject[] skyObjects = location.getSkyObjects();

  for(int iObject = 0; iObject < skyObjects.length; ++iObject) {
    SkyObject object = skyObjects[iObject];
    renderSystem.setMaterialTexture(object.texture);
    renderSystem.setMesh(object.mesh);
    renderSystem.renderMesh(WORKAROUND_scaleMatrix);
  }  

  // ----------------------------------------
  // Render Planet

  PlanetPatch[] visiblePatches;
  PlanetPatch[] patches = location.getPlanetPatches();

  // Culling here
  Matrix planetRotate = location.getPlanetRotate();
  for(int iPatch = 0; iPatch < patches.length; ++iPatch) {
    PlanetPatch patch = patches[iPatch];
    Vector position = patch.groundMesh.getMeshCenter();
    position.normalize();
    position = position * planetRotate;
    if(acos(position.dotProduct(Vector.UNIT_Y)) < PLANET_ANGLE) {
      // Visible, force loading and add to renderable
      location.touchPatch(iPatch);
      visiblePatches ~= patch;
    }
  }
  // Sort patches here
  // zzz

  // Render ground
  Matrix planetTransform = location.getPlanetTransform();  
  for(int iPatch = 0; iPatch < visiblePatches.length; ++iPatch) {
    PlanetPatch patch = visiblePatches[iPatch];
    renderSystem.setMaterialTexture(patch.groundTexture);
    renderSystem.setMesh(patch.groundMesh);
    renderSystem.renderMesh(planetTransform);
  }
  
  // Render ground haze
  if(location.getGroundHazeTexture() != null && location.getGroundHazeMesh() != null) {
    renderSystem.setMaterialTexture(location.getGroundHazeTexture());
    renderSystem.setMesh(location.getGroundHazeMesh(), false, false);
    renderSystem.renderMesh(Matrix.IDENTITY);
  }
  
  // Render clouds
  for(int iPatch = 0; iPatch < visiblePatches.length; ++iPatch) {
    PlanetPatch patch = visiblePatches[iPatch];
    renderSystem.setMaterialTexture(patch.cloudTexture);
    renderSystem.setMesh(patch.cloudMesh);
    renderSystem.renderMesh(planetTransform);
  }
  
  // Render cloud haze
  //   if(location.getCloudHazeTexture() != null && location.getCloudHazeMesh() != null) {
  //     renderSystem.setMaterialTexture(location.getCloudHazeTexture());
  //     renderSystem.setMesh(location.getCloudHazeMesh());
  //     renderSystem.renderMesh(Matrix::IDENTITY);
  //   }


  renderSystem.enableAlphaBlending(false);
  renderSystem.enableZBuffer(true);
}


