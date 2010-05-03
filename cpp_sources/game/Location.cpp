//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"


#include "Location.hpp"
#include "Station.hpp"

#include "file/CacheManager.hpp"
#include "file/ChunkData.hpp"
#include "file/ChunkStream.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/Texture.hpp"

const float PLANET_RENDERING_SCALE = 0.00001f;

//--------------------------------------------------------------------------------

Location::Location()
  : mGroundHazeMesh(NULL), mGroundHazeTexture(NULL), mCloudHazeMesh(NULL), mCloudHazeTexture(NULL),
    mChunkData(NULL)
{
  mGroundHazeMesh = rRenderSystem()->createMesh();
  mGroundHazeTexture = rRenderSystem()->createTexture();
  mCloudHazeMesh = rRenderSystem()->createMesh();
  mCloudHazeTexture = rRenderSystem()->createTexture();
  clear();
}

Location::~Location()
{
  // Release ChunkData based on filename
}

// --------------------------------------------------------------------------------

bool
Location::load(string fileName)
{
  clear();
  if(!rCacheManager()->aquireChunkData(fileName, mChunkData)) {
    logEngineError("Unable to load Location");
    return false;
  }

  ChunkIterator curr = mChunkData->begin();

  curr = mChunkData->begin();
  while(curr.getChunkType() != "MESH" && curr != mChunkData->end()) {
  // Skip CHUNK we don't understand yet
    ++curr;
  }
  if(curr == mChunkData->end()) // ERROR
    return false;
  
  // Read SkyObjects
  while(curr.getChunkType() != "PLNT" && curr != mChunkData->end()) {
    SkyObject object;
    object.mesh = rRenderSystem()->createMesh();
    object.texture = rRenderSystem()->createTexture();
    if(!object.mesh->load(curr, mChunkData->end(), curr)) {
      logEngineError("Error loading SkyObject mesh");
      return false;
    }
    if(!object.texture->load(curr, mChunkData->end(), curr)) {
      logEngineError("Error loading SkyObject texture");
      return false;
    }
    
    mSkyObjects.push_back(object);
  }
  
  // Read planet
  if(curr.getChunkType() != "PLNT") // ERROR
    return false;

  curr.read(mPlanetRadius);
  curr.read(mOrbitRadius);
  curr.read(mPlanetAngleSpeed);
  int numPatches;
  curr.read(numPatches);
  ++curr;
  
  // Read haze layers
  if(curr == mChunkData->end() || curr.getChunkType() != "IRGB") // ERROR
    return false;
  mGroundHazeTexture->load(curr, mChunkData->end(), curr);  
  if(curr == mChunkData->end() || curr.getChunkType() != "IRGB") // ERROR
    return false;
  mCloudHazeTexture->load(curr, mChunkData->end(), curr);

  for(int iPatch = 0; iPatch < numPatches; ++iPatch) {
    PlanetPatch patch;
    patch.loaded = false;
    patch.patchIterator = curr;
    
    patch.groundMesh = rRenderSystem()->createMesh();
    patch.cloudMesh = rRenderSystem()->createMesh();
    patch.groundTexture = rRenderSystem()->createTexture();
    patch.cloudTexture = rRenderSystem()->createTexture();
    
    if(curr == mChunkData->end() || curr.getChunkType() != "MESH") return false;
    patch.groundMesh->load(curr, mChunkData->end(), curr);
    if(curr == mChunkData->end() || curr.getChunkType() != "IRGB") return false;
    ++curr;
    if(curr == mChunkData->end() || curr.getChunkType() != "MESH") return false;
    patch.cloudMesh->load(curr, mChunkData->end(), curr);
    if(curr == mChunkData->end() || curr.getChunkType() != "IRGB") return false;
    ++curr;

    mPlanetPatches.push_back(patch);

  }
  
  updateHazeMeshes();
  
  return true;
}

// --------------------------------------------------------------------------------

void
Location::touchPatch(int iPatch)
{
  PlanetPatch& patch = mPlanetPatches[iPatch];
  if(patch.loaded || mChunkData == NULL) return;

  // Loading a patch
  ChunkIterator curr = patch.patchIterator;
  ++curr;
  patch.groundTexture->load(curr, mChunkData->end(), curr);
  ++curr;
  patch.cloudTexture->load(curr, mChunkData->end(), curr);    
  patch.loaded = true;
}

// --------------------------------------------------------------------------------

bool
Location::save(ChunkStream* chunkStream)
{
  for(SkyObjectList::iterator iObject = mSkyObjects.begin(); iObject != mSkyObjects.end(); ++iObject) {
    SkyObject& object = *iObject;
    object.mesh->save(chunkStream);
    object.texture->save(chunkStream, COMPRESSION_RAW);
  }

  chunkStream->openChunk("PLNT");
  chunkStream->write(mPlanetRadius);
  chunkStream->write(mOrbitRadius);
  chunkStream->write(mPlanetAngleSpeed);
  chunkStream->write(mPlanetPatches.size());
  chunkStream->closeChunk();
  
  mGroundHazeTexture->save(chunkStream, COMPRESSION_RAW);
  mCloudHazeTexture->save(chunkStream, COMPRESSION_RAW);

  for(PlanetPatchList::iterator iPatch = mPlanetPatches.begin(); iPatch != mPlanetPatches.end(); ++iPatch) {
    PlanetPatch& patch = *iPatch;
    patch.groundMesh->save(chunkStream);
    patch.groundTexture->save(chunkStream, COMPRESSION_RAW);
    patch.cloudMesh->save(chunkStream);
    patch.cloudTexture->save(chunkStream, COMPRESSION_RAW);
  }
  
  return true;
}


// --------------------------------------------------------------------------------

Matrix
Location::getPlanetTransform()
{
  Matrix orbitTranslate(Vector(0, -mOrbitRadius * PLANET_RENDERING_SCALE, 0));
  Matrix orbitRotate(Quaternion(Vector::UNIT_X, mPlanetAnglePosition));
  return orbitRotate * orbitTranslate;
}

Matrix
Location::getPlanetRotate()
{
  return Matrix(Quaternion(Vector::UNIT_X, mPlanetAnglePosition));
}




// --------------------------------------------------------------------------------

void
Location::update(float timeStep)
{
  mPlanetAnglePosition += mPlanetAngleSpeed * timeStep;
}


// --------------------------------------------------------------------------------

void
Location::clear()
{
  for(SkyObjectList::iterator iObject = mSkyObjects.begin(); iObject != mSkyObjects.end(); ++iObject) {
    SkyObject& object = *iObject;
    rRenderSystem()->destroyMesh(object.mesh);
    rRenderSystem()->destroyTexture(object.texture);
  }
  mSkyObjects.clear();

  for(PlanetPatchList::iterator iPatch = mPlanetPatches.begin(); iPatch != mPlanetPatches.end(); ++iPatch) {
    PlanetPatch& patch = *iPatch;
    rRenderSystem()->destroyMesh(patch.groundMesh);
    rRenderSystem()->destroyTexture(patch.groundTexture);
    rRenderSystem()->destroyMesh(patch.cloudMesh);
    rRenderSystem()->destroyTexture(patch.cloudTexture);
  }
  mPlanetPatches.clear();

  mPlanetRadius = 0;
  mOrbitRadius = 0;
  mPlanetAnglePosition = 0;
  mPlanetAngleSpeed = 0;

  mGroundHazeTexture->loadCheckerBoard();
  mCloudHazeTexture->loadCheckerBoard();  

  if(mChunkData != NULL) {
    delete mChunkData;
    mChunkData = NULL;
  }  
}


// --------------------------------------------------------------------------------


const int STAR_NUM = 3000;
const float STAR_LARGE_PROBABILITY = 0.15;

const float EARTH_RADIUS = 6378.1e3f;
const float EARTH_ORBIT_PERIOD = 90;
const float EARTH_ANGLE_SPEED = 3.14159 * 2.0f / (EARTH_ORBIT_PERIOD * 60.0f);

void
Location::loadDefaultLocation()
{
  clear();

  // Random star distribution
  {
    SkyObject starObject;
    starObject.mesh = rRenderSystem()->createMesh();
    VertexData& starVertices = starObject.mesh->getVertexData();
    IndexData& starIndices = starObject.mesh->getIndexData();
    for(int iStar = 0; iStar < STAR_NUM; ++iStar) {
      float distance = 200 + frand() * 200;
      Vector pos1(3.5f, 1.0f, distance);
      Vector pos2(5.0f, -5.0f, distance);
      Vector pos3(-2.0f, -2.0f, distance);

      float randomAngle = frand();
      float xAngle = randomAngle * randomAngle * Math::PI * 0.65 - Math::PI * 0.15; // Under horizon and weighted down
      Matrix transform1(Quaternion(Vector::UNIT_X, -xAngle));
      Matrix transform2(Quaternion(Vector::UNIT_Y, frand() * Math::PI * 2));
      Vertex starVertex1, starVertex2, starVertex3;
      starVertex1.position = pos1 * transform1 * transform2;
      starVertex2.position = pos2 * transform1 * transform2;
      starVertex3.position = pos3 * transform1 * transform2;
      if(frand() < STAR_LARGE_PROBABILITY) {
        starVertex1.u = 1;
        starVertex1.v = 0;
        starVertex2.u = 1;
        starVertex2.v = 1;
        starVertex3.u = 0;
        starVertex3.v = 0;
      }
      else {
        starVertex1.u = 0;
        starVertex1.v = 1;
        starVertex2.u = 1;
        starVertex2.v = 1;
        starVertex3.u = 0;
        starVertex3.v = 0;        
      }
      starVertices.push_back(starVertex1);
      starVertices.push_back(starVertex2);
      starVertices.push_back(starVertex3);
      starIndices.push_back(iStar * 3);
      starIndices.push_back(iStar * 3 + 1);
      starIndices.push_back(iStar * 3 + 2);
    }
    starObject.mesh->flush();
    starObject.texture = rRenderSystem()->createTexture();
    starObject.texture->loadFromPNG("assets/default/stars.png");
    mSkyObjects.push_back(starObject);
  }

  // Earth like planet  
  mPlanetRadius = EARTH_RADIUS;
  mOrbitRadius = EARTH_RADIUS + 400e3f;
  mPlanetAnglePosition = 0;
  mPlanetAngleSpeed = EARTH_ANGLE_SPEED;
  
  mGroundHazeTexture->loadFromPNG("assets/default/earth_ground_haze.png");
  //  mGroundHazeTexture->loadCheckerBoard();
  mCloudHazeTexture->loadFromPNG("assets/default/earth_cloud_haze.png");

  const int PATCH_NUMBER = 10;
  const int PATCH_SIZE = 15;
//   const float PATCH_WIDTH = 0.5f;

  float renderRadius = mPlanetRadius * PLANET_RENDERING_SCALE;
  
  float angleStep = Math::PI * 2.0f / (PATCH_NUMBER * PATCH_SIZE);
  for(int iPatch = 0; iPatch < PATCH_NUMBER; ++iPatch) {
    Mesh* patchMesh = rRenderSystem()->createMesh();

    // Vertices
    float longitude = iPatch * PATCH_SIZE * angleStep;
    for(int gridW = 0; gridW < (PATCH_SIZE + 1); ++gridW) {
      float latitude = (-PATCH_SIZE / 2) * angleStep;
      for(int gridH = 0; gridH < (PATCH_SIZE + 1); ++gridH) {
        Vertex patchVertex;
        patchVertex.position.x = renderRadius * sinf(latitude);
        patchVertex.position.y = renderRadius * cosf(latitude) * sinf(longitude);
        patchVertex.position.z = renderRadius * cosf(latitude) * cosf(longitude);
        patchVertex.u = static_cast<float>(gridW) / static_cast<float>(PATCH_SIZE);
        patchVertex.v = static_cast<float>(gridH) / static_cast<float>(PATCH_SIZE);        
        patchMesh->getVertexData().push_back(patchVertex);
        latitude += angleStep;
      }
      longitude += angleStep;
    }

    // Indices
    for(int gridW = 0; gridW < PATCH_SIZE; ++gridW) {
      for(int gridH = 0; gridH < PATCH_SIZE; ++gridH) {
        int topLeft = gridW + gridH * (PATCH_SIZE + 1);
        int topRight = (gridW + 1) + gridH * (PATCH_SIZE + 1);
        int bottomLeft = gridW + (gridH + 1) * (PATCH_SIZE + 1);
        int bottomRight = (gridW + 1) + (gridH + 1) * (PATCH_SIZE + 1);
        IndexData& indexData = patchMesh->getIndexData();

        indexData.push_back(topRight);
        indexData.push_back(bottomLeft);
        indexData.push_back(topLeft);

        indexData.push_back(bottomRight);
        indexData.push_back(bottomLeft);
        indexData.push_back(topRight);
      }
    }
    patchMesh->flush();

    PlanetPatch patch;
    patch.loaded = true;
    patch.groundMesh = patchMesh;
    // FIXME: Clouds should have separate mesh: This will crash
    patch.cloudMesh = patchMesh;

    patch.groundTexture = rRenderSystem()->createTexture();
    patch.groundTexture->loadFromPNG("assets/default/earth_ground.png");
    patch.cloudTexture = rRenderSystem()->createTexture();
    patch.cloudTexture->loadFromPNG("assets/default/earth_clouds.png");
    mPlanetPatches.push_back(patch);
  }
  
  
  updateHazeMeshes();
}

// --------------------------------------------------------------------------------

void
Location::updateHazeMeshes()
{
  const int SEGMENT_NUMBER = 64;
  
  VertexData& groundHazeVertices = mGroundHazeMesh->getVertexData();
  IndexData& groundHazeIndices = mGroundHazeMesh->getIndexData();
  groundHazeVertices.resize((SEGMENT_NUMBER + 1) * 2);
  groundHazeIndices.resize(SEGMENT_NUMBER * 6);

  float groundHazeInnerPlane = -120;
  float groundHazeOuterPlane = -40;

  float groundHazeInnerRadius = 40;
  float groundHazeOuterRadius = 140;

  for(int iSegment = 0; iSegment < (SEGMENT_NUMBER + 1); ++iSegment) {
    Vertex innerVertex;
    Vertex outerVertex;
    
    float segmentAngle = (iSegment * Math::PI * 2.0f) / SEGMENT_NUMBER;
    innerVertex.position = Vector(cosf(segmentAngle) * groundHazeInnerRadius, groundHazeInnerPlane, sinf(segmentAngle) * groundHazeInnerRadius);
    outerVertex.position = Vector(cosf(segmentAngle) * groundHazeOuterRadius, groundHazeOuterPlane, sinf(segmentAngle) * groundHazeOuterRadius);
    
    innerVertex.normal = Vector::UNIT_Y;
    outerVertex.normal = Vector::UNIT_Y;
    
    float textureV = static_cast<float>(iSegment) / SEGMENT_NUMBER;
    textureV = clamp<float>(textureV, 0.0f, 1.0f);
    textureV *= 4;
    innerVertex.u = 0;
    innerVertex.v = textureV;
    outerVertex.u = 1;
    outerVertex.v = textureV;
    
    groundHazeVertices[iSegment * 2] = innerVertex;
    groundHazeVertices[iSegment * 2 + 1] = outerVertex;
  }

  for(int iSegment = 0; iSegment < SEGMENT_NUMBER; ++iSegment) {
    int index = iSegment * 6;
    int topLeft = iSegment * 2 + 1;
    int bottomLeft = iSegment * 2;
    int topRight = (iSegment + 1) * 2 + 1;
    int bottomRight = (iSegment + 1) * 2;

    groundHazeIndices[index] = topLeft;
    groundHazeIndices[index + 1] = topRight;
    groundHazeIndices[index + 2] = bottomLeft;

    groundHazeIndices[index + 3] = topRight;
    groundHazeIndices[index + 4] = bottomRight;
    groundHazeIndices[index + 5] = bottomLeft;
  }  

  mGroundHazeMesh->flush();
  mCloudHazeMesh = mGroundHazeMesh;
}


