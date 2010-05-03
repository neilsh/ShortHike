//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef GAME_LOCATION_HPP
#define GAME_LOCATION_HPP

#include "file/ChunkData.hpp"

class Mesh;
class Texture;
class ChunkStream;
class ChunkData;
class ChunkIterator;

struct SkyObject
{
  Mesh* mesh;
  Texture* texture;
};

struct PlanetPatch
{
  bool loaded;
  ChunkIterator patchIterator;
  Mesh* groundMesh;
  Texture* groundTexture;
  Mesh* cloudMesh;
  Texture* cloudTexture;
};

typedef vector<SkyObject> SkyObjectList;
typedef vector<PlanetPatch> PlanetPatchList;

class Location
{
public:
  Location();
  ~Location();

  void update(float timeStep);

  const SkyObjectList& getSkyObjects() {return mSkyObjects;}

  float getPlanetRadius() {return mPlanetRadius;}
  float getPlanetAnglePosition() {return mPlanetAnglePosition;}
  float getPlanetAngleSpeed() {return mPlanetAngleSpeed;}

  Matrix getPlanetTransform();
  Matrix getPlanetRotate();

  Mesh* getGroundHazeMesh() {return mGroundHazeMesh;}
  Mesh* getCloudHazeMesh() {return mCloudHazeMesh;}
  Texture* getGroundHazeTexture() {return mGroundHazeTexture;}
  Texture* getCloudHazeTexture() {return mCloudHazeTexture;}

  void clear();
  void loadDefaultLocation();
  
  bool load(string fileName);
  bool save(ChunkStream* chunkStream);

  const PlanetPatchList& getPlanetPatches() {return mPlanetPatches;}  
  void touchPatch(int patchIndex);
private:
  void updateHazeMeshes();

  vector<SkyObject> mSkyObjects;
  
  float mPlanetRadius;
  float mOrbitRadius;
  float mPlanetAnglePosition;
  float mPlanetAngleSpeed;
  
  Mesh* mGroundHazeMesh;
  Texture* mGroundHazeTexture;
  Mesh* mCloudHazeMesh;
  Texture* mCloudHazeTexture;  
  vector<PlanetPatch> mPlanetPatches;

  ChunkData* mChunkData;
};


#endif
