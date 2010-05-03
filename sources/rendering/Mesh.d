//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
// NOTE: Transparent surfaces are ordered last in the index list. Then they can be
// rendered as a batch postprocess. Only convex transparency that bounds to the
// module is supported.

private import Common;
private import KDTree;
private import ChunkData;
private import CacheManager;
private import ChunkStream;
private import std.string;
private import std.stream;

// --------------------------------------------------------------------------------

struct Vertex
{
public:
  Vector position = Vector.ZERO;
  Vector normal = Vector.ZERO;
  float u = 0;
  float v = 0;
  union 
  {
    struct 
      {
        byte primary_bone, primary_weight, secondary_bone, secondary_weight;
      };
    uint anim_info;
  }
}

// --------------------------------------------------------------------------------


class Mesh
{
public:

  // --------------------------------------------------------------------------------
  
  this(bool collisionSupport)
  {
    mCollisionSupport = collisionSupport;
    if(mCollisionSupport)
      mCollisionKDTree = new KDTree();
  }
  
  ~this()
  {
  }

  // --------------------------------------------------------------------------------

  bool
  load(char[] fileName)
  {
    ChunkData chunkData;
    if(!rCacheManager().aquireChunkData(fileName, chunkData)) {
      logError << "Unable to load Module:" << fileName << endl;
      return false;
    }
    
    ChunkIterator curr = chunkData.begin();

    curr = chunkData.begin();
    // Skip non-MESH chunks
    while(curr.getChunkType() != "MESH" && curr != chunkData.end()) {
      curr++;
    }

    // No mesh chunk found
    if(curr == chunkData.end()) {
      return false;
    }
  
    load(curr, chunkData.end(), curr);

    rCacheManager().releaseChunkData(fileName);
    return true;
  }


  // --------------------------------------------------------------------------------

  const int MAXIMUM_VERTEX_COUNT = 1000000;
  const int MAXIMUM_INDEX_COUNT = 1000000;

  bool
  load(ChunkIterator begin, ChunkIterator end, out ChunkIterator curr)
  {
    curr = begin;
    if(curr == end || curr.getChunkType() != "MESH") return false;
  
    int vertexCount;
    int indexCount;
    int transparencyIndex;
    curr.read(vertexCount);
    curr.read(indexCount);

    if(vertexCount < 0 || vertexCount > MAXIMUM_VERTEX_COUNT ||
       indexCount < 0 || indexCount > MAXIMUM_INDEX_COUNT) {
      logError << "Malformed MESH chunk" << endl;
      return false;
    }

    curr.readExact(cast(ubyte*)&mBoundingBox, mBoundingBox.sizeof);

    curr.read(transparencyIndex);
    if(transparencyIndex != indexCount) {
      logWarn << "Transparency not supported" << endl;
    }
    mVertexData.length = vertexCount;

    ubyte[] vertexTemp = cast(ubyte[])mVertexData;
    if(!curr.readCompressed(vertexTemp)) {
      mVertexData = mVertexData.init;
      return false;
    }

    mIndexData.length = indexCount;
    ubyte[] indexTemp = cast(ubyte[])mIndexData;
    if(!curr.readCompressed(indexTemp)) {
      mIndexData = mIndexData.init;
      return false;
    }
  
    curr++;
    if(curr != end && curr.getChunkType() == "COLL") {
      if(mCollisionSupport) {
        if(!mCollisionKDTree.load(curr, end, curr)) {
          flushCollision();
        }
      }
      else {
        curr++;
      }
    }
    flushInternal();
    
    return true;
  }


  // --------------------------------------------------------------------------------

  bool 
  save(ChunkStream stream)
  {
    stream.openChunk("MESH");
    int vertexCount = getVertexCount();
    int indexCount = getIndexCount();
    
    stream.write(vertexCount);
    stream.write(indexCount);
    
    stream.writeExact(&mBoundingBox, mBoundingBox.sizeof);
    
    // Future support for transparent surfaces
    int transparencyIndex = indexCount;
    stream.write(transparencyIndex);
    
    stream.writeCompressedDeflate(cast(byte*)mVertexData.ptr, mVertexData[0].sizeof * vertexCount);
    stream.writeCompressedDeflate(cast(byte*)mIndexData.ptr, int.sizeof * indexCount);
    stream.closeChunk();  
    
    if(mCollisionSupport)
      mCollisionKDTree.save(stream);
    return true;
  }


  // --------------------------------------------------------------------------------

  void
  flush()
  {
    mBoundingBox.clear();
    for(int iVertex = 0; iVertex< mVertexData.length; ++iVertex) {
      mBoundingBox.addPoint(mVertexData[iVertex].position);
    }  
    flushCollision();
    flushInternal();
  }


  void
  flushCollision()
  {
    if(mCollisionSupport) {
      KDTriList triangles;
      for(int iIndex = 0; iIndex < mIndexData.length / 3; ++iIndex) {
        int baseIndex = iIndex * 3;
        KDTriangle triangle;
        Vector pos0 = mVertexData[mIndexData[baseIndex]].position;
        Vector pos1 = mVertexData[mIndexData[baseIndex + 1]].position;
        Vector pos2 = mVertexData[mIndexData[baseIndex + 2]].position;
        triangle.vert0 = pos0;
        triangle.edge1 = pos1 - pos0;
        triangle.edge2 = pos2 - pos0;
        triangle.index = iIndex;
        triangles ~= triangle;
      }
      mCollisionKDTree.createTree(mBoundingBox, triangles);
    }
  }
  
  // --------------------------------------------------------------------------------

  int getVertexCount() {
    return mVertexData.length;
  }
  
  int getIndexCount() {
    return mIndexData.length;
  }

  Vector
  getMeshCenter()
  {
    return (mBoundingBox.minPoint + mBoundingBox.maxPoint) * 0.5f;
  }

  float
  getBoundingRadius()
  {
    return (mBoundingBox.minPoint - mBoundingBox.maxPoint).length() * 0.5f;
  }
  
  AxisAlignedBox
  getBoundingBox()
  {
    return mBoundingBox;
  }
  
  // --------------------------------------------------------------------------------

  void
  translate(Vector delta)
  {
    foreach(inout Vertex v; mVertexData) {
      v.position += delta;
    }    
    flush();
  }
  

  // --------------------------------------------------------------------------------

  bool
  intersectRayBox(Ray pickRay, Matrix transform, out float oDistance)
  {
    if(!mCollisionSupport) return false;
    
    Matrix inverseTransform = transform.inverse();
    
    Ray localRay;
    localRay.origin = pickRay.origin * inverseTransform;
    Vector zero = Vector.ZERO;
    inverseTransform.setTranslation(Vector.ZERO);
    localRay.direction = pickRay.direction * inverseTransform;
    
    float farDistance = 0;
    return mBoundingBox.intersect(localRay, oDistance, farDistance);
  }

  // --------------------------------------------------------------------------------
  
  bool
  intersectRayTri(Ray pickRay, Matrix transform, out float oDistance)
  {
    if(!mCollisionSupport) return false;
    
    Vector pos1, pos2, pos3;
    Matrix inverseTransform = transform.inverse();
    Ray localRay;
    localRay.origin = pickRay.getOrigin() * inverseTransform;
    inverseTransform.setTranslation(Vector.ZERO);
    localRay.direction = pickRay.getDirection() * inverseTransform;
    localRay.direction.normalize();
    return mCollisionKDTree.intersectRay(localRay, oDistance);
  }


  // --------------------------------------------------------------------------------
  // Loading from an OBJ data file

  struct ObjTex
  {
    float u, v;
  };
  
  struct ObjVertex
  {
    int iPos;
    int iTexture;
    int iNormal;
  };
  
  // bool
  // operator<(const ObjVertex& vertex1, const ObjVertex& vertex2)
  // {
  //   if(vertex1.iPos != vertex2.iPos) {
  //     return  vertex1.iPos < vertex2.iPos;
  //   }
  //   else if(vertex1.iTexture != vertex2.iTexture) {
  //     return vertex1.iTexture < vertex2.iTexture;
  //   }
  //   else {
  //     return vertex1.iNormal < vertex2.iNormal;
  //   }
  // }

  void
  skipSpace(File file)
  {
    char next = file.getc();
    while(next == ' ') {
      next = file.getc();
    }
    file.ungetc(next);
  }
  


  void
  loadFromOBJ(char[] fileName)
  {
    int[ObjVertex] vertexToIndex;
    Vector[] positions;
    Vector[] normals;
    ObjTex[] textures;
  
    mVertexData.length = 0;
    mIndexData.length = 0;
    
    int line = 1;
    File file;
    try {
      file = new File(fileName, FileMode.In);
    }
    catch(Exception error) {
      logError << "Unable to open OBJ file: " << fileName << endl;
      return;
    }
    
  
    while(!file.eof()) {
      char nextChar;
      nextChar = file.getc();

      if('#' == nextChar) {
        file.readLine();
        line++;
        continue;
      }
      else if('\r' == nextChar || ' ' == nextChar || '\t' == nextChar || '\n' == nextChar) {
        continue;
      }

      file.ungetc(nextChar);
      char[] token;
      file.readf("%s", &token);
      if("v" == token) {
        Vector pos;
        file.readf("%f %f %f", &pos.x, &pos.y, &pos.z);
        pos.x = -pos.x;
        positions ~= pos;
        file.readLine();
      }
      else if("vn" == token) {
        Vector normal;
        file.readf("%f %f %f", &normal.x, &normal.y, &normal.z);
        normal.x = -normal.x;
        normals ~= normal;
        file.readLine();
      }
      else if("vt" == token) {
        ObjTex tex;
        file.readf("%f %f", &tex.u, &tex.v);
        textures ~= tex;        
        file.readLine();
      }
      else if("f" == token) {
        int count = 0;
        int vertices[3];
        ObjVertex objVertex;        
        skipSpace(file);
        char separator = file.getc();
        while(isNumeric(separator)) {
          file.ungetc(separator);
          file.readf("%d", &objVertex.iPos);
          separator = file.getc();
          if('/' == separator) {
            separator = file.getc();
            if('/' == separator) {
              objVertex.iTexture = -1;
            }
            else {
              file.ungetc(separator);
              file.readf("%d", &objVertex.iTexture);
            }
            
            separator = file.getc();
            if('/' != separator) {
              logError << "OBJ Loader: " << fileName << "(" << line << ") : only 'f n ..' or 'f n/n/n ..' forms supported" << endl;
              break;
            }
            file.readf("%d",  &objVertex.iNormal);
          }
          else {
            objVertex.iTexture = objVertex.iPos;
            objVertex.iNormal = objVertex.iPos;
          }

          // Handle actual triangle addition
          int indexPlusOne;
          if(objVertex in vertexToIndex is null) {
            //             logInfo << "Position: " << objVertex.iPos << "/" << positions.length
            //                     << " Normal: " << objVertex.iNormal << "/" << normals.length
            //                     << "Texture: " << objVertex.iTexture << "/" << textures.length << endl;
            Vertex vertex;
            vertex.position = positions[objVertex.iPos - 1];
            vertex.normal = normals[objVertex.iNormal - 1];
            if(objVertex.iTexture != -1) {
              ObjTex tex = textures[objVertex.iTexture - 1];
              vertex.u = tex.u; vertex.v = tex.v;          
            }
            else {
              vertex.u = 0; vertex.v = 0;
            }
            
            //             logInfo << "Pushing vertex" << endl;
            mVertexData ~= vertex;
            indexPlusOne = mVertexData.length;
            vertexToIndex[objVertex] = indexPlusOne;
          }
          indexPlusOne = vertexToIndex[objVertex];

          if(count >= 3) {
            vertices[1] = vertices[2];
            vertices[2] = indexPlusOne - 1;
          }
          else {
            vertices[count] = indexPlusOne - 1;
          }        
          if(count >= 2) {
            //             logInfo << "Pushing triangle" << endl;
            mIndexData ~= vertices[2];
            mIndexData ~= vertices[1];
            mIndexData ~= vertices[0];
            //           mIndexData.push_back(vertices[2]);
            //           mIndexData.push_back(vertices[1]);
            //           mIndexData.push_back(vertices[0]);
          }
          count++;
          skipSpace(file);
          separator = file.getc();
        }
        // This might not be enough to skip end of line? Are comments accepted anywhere?
        //       file.clear();
        file.ungetc(separator);
        file.readLine();
      }
      else {
        logWarn << "OBJ: Loader: " << fileName << "(" << line << ") : Unknown token'" << token << "'" << endl;
        file.readLine();
      }
      line ++;
    }

    flush();
  }


  // --------------------------------------------------------------------------------
  // Loading from an SSM Mesh data file


  // bool
  // operator<(const Vertex& lhs, const Vertex& rhs)
  // {
  //   if(lhs.position != rhs.position)
  //     return lhs.position < rhs.position;
  //   if(lhs.normal != rhs.normal)
  //     return lhs.normal < rhs.normal;

  //   if(lhs.u != rhs.u)
  //     return lhs.u < rhs.u;

  //   return lhs.v < rhs.v;
  // }



  void
  loadFromSSM(char[] fileName)
  {
    assert(CHECK_FAIL("Not implemented"));
  
    //   map<Vertex, int> vertexToIndex;
  
    //   const int MAX_LINE_SKIP = 1000;

    // //   int line = 1;
    //   ifstream file(fileName.c_str());
    //   mVertexData.clear();
    //   mIndexData.clear();
  
    //   int portNum;
    //   file >> portNum;
    //   while(file.peek() == '\n') file.ignore(MAX_LINE_SKIP, '\n');
    //   for(int skipPort = 0; skipPort < portNum; ++skipPort) {
    //     file.ignore(MAX_LINE_SKIP, '\n');
    //   }
    //   int materialNum;
    //   file >> materialNum;
    //   if(materialNum > 1) {
    //     logEngineWarn("SSM Mesh Loader: Too many materials in mesh file");
    //   }
    //   string materialName;
    //   while(file.peek() == '\n')
    //     file.ignore(MAX_LINE_SKIP, '\n'); // There is an odd empty line here..
    //   for(int skipMaterial = 0; skipMaterial < materialNum; ++skipMaterial)
    //     file.ignore(MAX_LINE_SKIP, '\n');

    //   int triangleCount;
    //   file >> triangleCount;
    //   int indexCount = triangleCount * 3;
    //   while(file.peek() == '\n') file.ignore(MAX_LINE_SKIP, '\n');
    //   for(int index = 0; index < indexCount; ++index) {
    //     Vertex vertex;
    //     file >> vertex.position.x >> vertex.position.y >> vertex.position.z
    //          >> vertex.normal.x >> vertex.normal.y >> vertex.normal.z
    //          >> vertex.u >> vertex.v;
    //     vertex.v = 1 - vertex.v;

    //     int indexPlusOne = vertexToIndex[vertex];
    //     if(0 == indexPlusOne) {
    //       mVertexData.push_back(vertex);
    //       indexPlusOne = mVertexData.size();
    //       vertexToIndex[vertex] = indexPlusOne;
    //     }
    //     mIndexData.push_back(indexPlusOne - 1);
    //     while(file.peek() == '\n') file.ignore(MAX_LINE_SKIP, '\n');
    //   }
    //   ostringstream ossLoadStat;
    //   ossLoadStat << "SSM Mesh Loader: " << mVertexData.size() << " vertices " << mIndexData.size() << " indices";
    //   logEngineInfo(ossLoadStat.str());

    //   flush();
  }


  // --------------------------------------------------------------------------------
  // Loading simple unit cube

  Vertex
  loadCubeVertex(Vector right, Vector up, Vector normal, bool isRight, bool isUp, float scale)
  {
    Vertex vertex;
    if(isRight) {
      vertex.position = right * scale;
    }
    else {
      vertex.position += -right * scale;
    }
    if(isUp) {
      vertex.position += up * scale;
    }
    else {
      vertex.position += -up * scale;
    }
    vertex.position += normal * scale;
    vertex.normal = normal;
    return vertex;
  }
  
  
  void
  loadCubeSurface(Vector right, Vector up, Vector normal, float scale)
  {
    Vertex ul = loadCubeVertex(right, up, normal, false, true, scale);
    Vertex ll = loadCubeVertex(right, up, normal, false, false, scale);
    Vertex lr = loadCubeVertex(right, up, normal, true, false, scale);
    Vertex ur = loadCubeVertex(right, up, normal, true, true, scale);
    
    ul.u = ll.u = 0;
    ur.u = lr.u = 1;
    ll.v = lr.v = 0;
    ul.v = ur.v = 1;
    
    int baseIndex = mVertexData.length;
    
    mVertexData ~= ul;
    mVertexData ~= ll;
    mVertexData ~= lr;
    mVertexData ~= ur;
    
    mIndexData ~= baseIndex + 2;
    mIndexData ~= baseIndex + 1;
    mIndexData ~= baseIndex + 0;
    
    mIndexData ~= baseIndex + 0;
    mIndexData ~= baseIndex + 3;
    mIndexData ~= baseIndex + 2;
  }
  

  void
  loadCube(float scale)
  {
    scale = scale * 0.5;
    mVertexData.length = 0;
    mIndexData.length = 0;
    
    loadCubeSurface(Vector.UNIT_X, Vector.UNIT_Y, -Vector.UNIT_Z, scale);
    loadCubeSurface(-Vector.UNIT_X, Vector.UNIT_Y, Vector.UNIT_Z, scale);
    
    loadCubeSurface(Vector.UNIT_Z, Vector.UNIT_Y, Vector.UNIT_X, scale);
    loadCubeSurface(-Vector.UNIT_Z, Vector.UNIT_Y, -Vector.UNIT_X, scale);
    
    loadCubeSurface(Vector.UNIT_X, Vector.UNIT_Z, Vector.UNIT_Y, scale);
    loadCubeSurface(Vector.UNIT_X, -Vector.UNIT_Z, -Vector.UNIT_Y, scale);
    
    flush();
  }
  
  // --------------------------------------------------------------------------------
  
  Vertex[] mVertexData;
  uint[] mIndexData;
protected:
  abstract void flushInternal();
  
  
  AxisAlignedBox mBoundingBox;
  
  bool mCollisionSupport;
  KDTree mCollisionKDTree;
}


