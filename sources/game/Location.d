//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Mesh;
private import Texture;
private import ChunkData;
private import ChunkStream;
private import ChunkCommon;
private import RenderSystem;
private import CacheManager;


struct SkyObject
{
  Mesh mesh;
  Texture texture;
}

struct PlanetPatch
{
  bool loaded;
  ChunkIterator patchIterator;
  Mesh groundMesh;
  Texture groundTexture;
  Mesh cloudMesh;
  Texture cloudTexture;
}



class Location
{
public:
  SkyObject[] getSkyObjects() {return mSkyObjects;}

  float getPlanetRadius() {return mPlanetRadius;}
  float getPlanetAnglePosition() {return mPlanetAnglePosition;}
  float getPlanetAngleSpeed() {return mPlanetAngleSpeed;}

  Mesh getGroundHazeMesh() {return mGroundHazeMesh;}
  Mesh getCloudHazeMesh() {return mCloudHazeMesh;}
  Texture getGroundHazeTexture() {return mGroundHazeTexture;}
  Texture getCloudHazeTexture() {return mCloudHazeTexture;}

  PlanetPatch[] getPlanetPatches() {return mPlanetPatches;}

  const float PLANET_RENDERING_SCALE = 0.00001f;

  //--------------------------------------------------------------------------------

  this()
  {
    mGroundHazeMesh = rRenderSystem().createMesh();
    mGroundHazeTexture = rRenderSystem().createTexture();
    mCloudHazeMesh = rRenderSystem().createMesh();
    mCloudHazeTexture = rRenderSystem().createTexture();
    clear();
  }

  this(char[] fileName)
  {
    this();
    load(fileName);
  }
  

  ~this()
  {
    // Release ChunkData based on filename
  }

  // --------------------------------------------------------------------------------

  bool
  load(char[] fileName)
  {
    clear();
    if(!rCacheManager().aquireChunkData(fileName, mChunkData)) {
      logError << "Unable to load Location" << endl;
      return false;
    }

    ChunkIterator curr = mChunkData.begin();

    curr = mChunkData.begin();
    while(curr.getChunkType() != "MESH" && curr != mChunkData.end()) {
      // Skip CHUNK we don't understand yet
      curr++;
    }
    if(curr == mChunkData.end()) // ERROR
      return false;
  
    // Read SkyObjects
    while(curr.getChunkType() != "PLNT" && curr != mChunkData.end()) {
      SkyObject object;
      object.mesh = rRenderSystem().createMesh();
      object.texture = rRenderSystem().createTexture();
      if(!object.mesh.load(curr, mChunkData.end(), curr)) {
        logError << "Error loading SkyObject mesh" << endl;
        return false;
      }
      if(!object.texture.load(curr, mChunkData.end(), curr)) {
        logError << "Error loading SkyObject texture" << endl;
        return false;
      }
    
      mSkyObjects ~= object;
    }
  
    // Read planet
    if(curr.getChunkType() != "PLNT") // ERROR
      return false;

    curr.read(mPlanetRadius);
    curr.read(mOrbitRadius);
    curr.read(mPlanetAngleSpeed);
    int numPatches;
    curr.read(numPatches);
    curr++;
  
    // Read haze layers
    if(curr == mChunkData.end() || curr.getChunkType() != "IRGB") // ERROR
      return false;
    mGroundHazeTexture.load(curr, mChunkData.end(), curr);  
    if(curr == mChunkData.end() || curr.getChunkType() != "IRGB") // ERROR
      return false;
    mCloudHazeTexture.load(curr, mChunkData.end(), curr);

    for(int iPatch = 0; iPatch < numPatches; ++iPatch) {
      PlanetPatch patch;
      patch.loaded = false;
      patch.patchIterator = curr.copy;
    
      patch.groundMesh = rRenderSystem().createMesh();
      patch.cloudMesh = rRenderSystem().createMesh();
      patch.groundTexture = rRenderSystem().createTexture();
      patch.cloudTexture = rRenderSystem().createTexture();
    
      if(curr == mChunkData.end() || curr.getChunkType() != "MESH") return false;
      patch.groundMesh.load(curr, mChunkData.end(), curr);
      if(curr == mChunkData.end() || curr.getChunkType() != "IRGB") return false;
      curr++;
      if(curr == mChunkData.end() || curr.getChunkType() != "MESH") return false;
      patch.cloudMesh.load(curr, mChunkData.end(), curr);
      if(curr == mChunkData.end() || curr.getChunkType() != "IRGB") return false;
      curr++;

      mPlanetPatches ~= patch;

    }
  
    updateHazeMeshes();
  
    return true;
  }

  // --------------------------------------------------------------------------------

  void
  touchPatch(int iPatch)
  {
    if(mPlanetPatches[iPatch].loaded || mChunkData == null) return;

    // Loading a patch
    ChunkIterator curr = mPlanetPatches[iPatch].patchIterator.copy;
    curr++;
    mPlanetPatches[iPatch].groundTexture.load(curr, mChunkData.end(), curr);
    curr++;
    mPlanetPatches[iPatch].cloudTexture.load(curr, mChunkData.end(), curr);
    mPlanetPatches[iPatch].loaded = true;
  }

  // --------------------------------------------------------------------------------

  bool
  save(ChunkStream chunkStream)
  {
    for(int iObject = 0; iObject < mSkyObjects.length; ++iObject) {
      SkyObject object = mSkyObjects[iObject];
      object.mesh.save(chunkStream);
      object.texture.save(chunkStream, CompressionType.COMPRESSION_RAW);
    }

    chunkStream.openChunk("PLNT");
    chunkStream.write(mPlanetRadius);
    chunkStream.write(mOrbitRadius);
    chunkStream.write(mPlanetAngleSpeed);
    chunkStream.write(mPlanetPatches.length);
    chunkStream.closeChunk();
  
    mGroundHazeTexture.save(chunkStream, CompressionType.COMPRESSION_RAW);
    mCloudHazeTexture.save(chunkStream, CompressionType.COMPRESSION_RAW);

    for(int iPatch = 0; iPatch < mPlanetPatches.length; ++iPatch) {
      PlanetPatch patch = mPlanetPatches[iPatch];
      patch.groundMesh.save(chunkStream);
      patch.groundTexture.save(chunkStream, CompressionType.COMPRESSION_RAW);
      patch.cloudMesh.save(chunkStream);
      patch.cloudTexture.save(chunkStream, CompressionType.COMPRESSION_RAW);
    }
  
    return true;
  }


  // --------------------------------------------------------------------------------

  Matrix
  getPlanetTransform()
  {
    Matrix orbitTranslate = createMatrix(createVector(0, -mOrbitRadius * PLANET_RENDERING_SCALE, 0));
    Matrix orbitRotate = createMatrix(createQuaternion(Vector.UNIT_X, mPlanetAnglePosition));
    return orbitRotate * orbitTranslate;
  }

  Matrix
  getPlanetRotate()
  {
    return createMatrix(createQuaternion(Vector.UNIT_X, mPlanetAnglePosition));
  }




  // --------------------------------------------------------------------------------

  void
  update(float timeStep)
  {
    mPlanetAnglePosition += mPlanetAngleSpeed * timeStep;
  }


  // --------------------------------------------------------------------------------

  void
  clear()
  {
    mSkyObjects.length = 0;
    mPlanetPatches.length = 0;

    mPlanetRadius = 0;
    mOrbitRadius = 0;
    mPlanetAnglePosition = 0;
    mPlanetAngleSpeed = 0;

    mGroundHazeTexture.loadCheckerBoard();
    mCloudHazeTexture.loadCheckerBoard();  
    mChunkData = null;
  }


  // --------------------------------------------------------------------------------


  const int STAR_NUM = 3000;
  const float STAR_LARGE_PROBABILITY = 0.15;

  const float EARTH_RADIUS = 6378.1e3f;
  const float EARTH_ORBIT_PERIOD = 90;
  const float EARTH_ANGLE_SPEED = 3.14159 * 2.0f / (EARTH_ORBIT_PERIOD * 60.0f);

  void
  loadDefaultLocation()
  {
    clear();

    // Random star distribution
    {
      SkyObject starObject;
      starObject.mesh = rRenderSystem().createMesh();
      Vertex[] starVertices = starObject.mesh.mVertexData;
      uint[] starIndices = starObject.mesh.mIndexData;
      for(int iStar = 0; iStar < STAR_NUM; ++iStar) {
        float distance = 200 + frand() * 200;
        Vector pos1 = createVector(3.5f, 1.0f, distance);
        Vector pos2 = createVector(5.0f, -5.0f, distance);
        Vector pos3 = createVector(-2.0f, -2.0f, distance);

        float randomAngle = frand();
        float xAngle = randomAngle * randomAngle * Math.PI * 0.65 - Math.PI * 0.15; // Under horizon and weighted down
        Matrix transform1 = createMatrix(createQuaternion(Vector.UNIT_X, -xAngle));
        Matrix transform2 = createMatrix(createQuaternion(Vector.UNIT_Y, frand() * Math.PI * 2));
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
        starVertices ~= starVertex1;
        starVertices ~= starVertex2;
        starVertices ~= starVertex3;
        starIndices ~= iStar * 3;
        starIndices ~= iStar * 3 + 1;
        starIndices ~= iStar * 3 + 2;
      }
      starObject.mesh.flush();
      starObject.texture = rRenderSystem().createTexture();
      starObject.texture.loadFromPNG("assets/default/stars.png");
      mSkyObjects ~= starObject;
    }

    // Earth like planet  
    mPlanetRadius = EARTH_RADIUS;
    mOrbitRadius = EARTH_RADIUS + 400e3f;
    mPlanetAnglePosition = 0;
    mPlanetAngleSpeed = EARTH_ANGLE_SPEED;
  
    mGroundHazeTexture.loadFromPNG("assets/default/earth_ground_haze.png");
    //  mGroundHazeTexture.loadCheckerBoard();
    mCloudHazeTexture.loadFromPNG("assets/default/earth_cloud_haze.png");

    const int PATCH_NUMBER = 10;
    const int PATCH_SIZE = 15;
    //   const float PATCH_WIDTH = 0.5f;

    float renderRadius = mPlanetRadius * PLANET_RENDERING_SCALE;
  
    float angleStep = Math.PI * 2.0f / (PATCH_NUMBER * PATCH_SIZE);
    for(int iPatch = 0; iPatch < PATCH_NUMBER; ++iPatch) {
      Mesh patchMesh = rRenderSystem().createMesh();

      // Vertices
      float longitude = iPatch * PATCH_SIZE * angleStep;
      for(int gridW = 0; gridW < (PATCH_SIZE + 1); ++gridW) {
        float latitude = (-PATCH_SIZE / 2) * angleStep;
        for(int gridH = 0; gridH < (PATCH_SIZE + 1); ++gridH) {
          Vertex patchVertex;
          patchVertex.position.x = renderRadius * sin(latitude);
          patchVertex.position.y = renderRadius * cos(latitude) * sin(longitude);
          patchVertex.position.z = renderRadius * cos(latitude) * cos(longitude);
          patchVertex.u = cast(float)gridW / cast(float)PATCH_SIZE;
          patchVertex.v = cast(float)gridH / cast(float)PATCH_SIZE;        
          patchMesh.mVertexData ~= patchVertex;
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
          uint[] indexData = patchMesh.mIndexData;

          indexData ~= topRight;
          indexData ~= bottomLeft;
          indexData ~= topLeft;

          indexData ~= bottomRight;
          indexData ~= bottomLeft;
          indexData ~= topRight;
        }
      }
      patchMesh.flush();

      PlanetPatch patch;
      patch.loaded = true;
      patch.groundMesh = patchMesh;
      // FIXME: Clouds should have separate mesh: This will crash
      patch.cloudMesh = patchMesh;

      patch.groundTexture = rRenderSystem().createTexture();
      patch.groundTexture.loadFromPNG("assets/default/earth_ground.png");
      patch.cloudTexture = rRenderSystem().createTexture();
      patch.cloudTexture.loadFromPNG("assets/default/earth_clouds.png");
      mPlanetPatches ~= patch;
    }
  
  
    updateHazeMeshes();
  }



  //--------------------------------------------------------------------------------

  LightEnv
  getLightEnv()
  {
    LightEnv env;
    env.primaryDirection = createVector(0.3, -1, 0.5);
    env.primaryColor = cColor(1, 1, 1);
    env.secondaryDirection = createVector(0, 1, 0);
    env.secondaryColor = cColor(0.55, 0.55, 0.65);
    return env;
  }


  // --------------------------------------------------------------------------------

  void
  updateHazeMeshes()
  {
    const int SEGMENT_NUMBER = 64;
  
    mGroundHazeMesh.mVertexData.length = (SEGMENT_NUMBER + 1) * 2;
    mGroundHazeMesh.mIndexData.length = SEGMENT_NUMBER * 6;

    float groundHazeInnerPlane = -120;
    float groundHazeOuterPlane = -40;

    float groundHazeInnerRadius = 40;
    float groundHazeOuterRadius = 140;

    for(int iSegment = 0; iSegment < (SEGMENT_NUMBER + 1); ++iSegment) {
      Vertex innerVertex;
      Vertex outerVertex;
    
      float segmentAngle = (iSegment * Math.PI * 2.0f) / SEGMENT_NUMBER;
      innerVertex.position = createVector(cos(segmentAngle) * groundHazeInnerRadius, groundHazeInnerPlane, sin(segmentAngle) * groundHazeInnerRadius);
      outerVertex.position = createVector(cos(segmentAngle) * groundHazeOuterRadius, groundHazeOuterPlane, sin(segmentAngle) * groundHazeOuterRadius);
    
      innerVertex.normal = Vector.UNIT_Y;
      outerVertex.normal = Vector.UNIT_Y;
    
      float textureV = cast(float)iSegment / SEGMENT_NUMBER;
      textureV = clamp(textureV, 0.0f, 1.0f);
      textureV *= 4;
      innerVertex.u = 0;
      innerVertex.v = textureV;
      outerVertex.u = 1;
      outerVertex.v = textureV;
    
      mGroundHazeMesh.mVertexData[iSegment * 2] = innerVertex;
      mGroundHazeMesh.mVertexData[iSegment * 2 + 1] = outerVertex;
    }

    for(int iSegment = 0; iSegment < SEGMENT_NUMBER; ++iSegment) {
      int index = iSegment * 6;
      int topLeft = iSegment * 2 + 1;
      int bottomLeft = iSegment * 2;
      int topRight = (iSegment + 1) * 2 + 1;
      int bottomRight = (iSegment + 1) * 2;

      mGroundHazeMesh.mIndexData[index] = topLeft;
      mGroundHazeMesh.mIndexData[index + 1] = topRight;
      mGroundHazeMesh.mIndexData[index + 2] = bottomLeft;

      mGroundHazeMesh.mIndexData[index + 3] = topRight;
      mGroundHazeMesh.mIndexData[index + 4] = bottomRight;
      mGroundHazeMesh.mIndexData[index + 5] = bottomLeft;
    }  

    mGroundHazeMesh.flush();
    mCloudHazeMesh = mGroundHazeMesh;
  }


private:
  SkyObject[] mSkyObjects;
  
  float mPlanetRadius;
  float mOrbitRadius;
  float mPlanetAnglePosition;
  float mPlanetAngleSpeed;
  
  Mesh mGroundHazeMesh;
  Texture mGroundHazeTexture;
  Mesh mCloudHazeMesh;
  Texture mCloudHazeTexture;  
  PlanetPatch[] mPlanetPatches;

  ChunkData mChunkData;
}

