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

#include "Common.hpp"
#include "Mesh.hpp"
#include "KDTree.hpp"

#include "file/CacheManager.hpp"
#include "file/ChunkStream.hpp"
#include "file/ChunkData.hpp"

// --------------------------------------------------------------------------------

Mesh::Mesh(bool collisionSupport)
  : mCollisionSupport(collisionSupport), mCollisionKDTree(NULL)
{
  if(mCollisionSupport)
    mCollisionKDTree = new KDTree();
}

Mesh::~Mesh()
{
}

// --------------------------------------------------------------------------------

bool
Mesh::load(string fileName)
{
  ChunkData* chunkData;
  if(!rCacheManager()->aquireChunkData(fileName, chunkData)) {
    logEngineError("Unable to load Module");
    return false;
  }
  
  ChunkIterator curr = chunkData->begin();

  curr = chunkData->begin();
  // Skip non-MESH chunks
  while(curr.getChunkType() != "MESH" && curr != chunkData->end()) {
    ++curr;
  }

  // No mesh chunk found
  if(curr == chunkData->end()) {
    delete chunkData;
    return false;
  }
  
  load(curr, chunkData->end(), curr);

  rCacheManager()->releaseChunkData(fileName);
  return true;
}


// --------------------------------------------------------------------------------

#pragma warning(disable: 4100)

const int MAXIMUM_VERTEX_COUNT = 1000000;
const int MAXIMUM_INDEX_COUNT = 1000000;

bool
Mesh::load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr)
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
    logEngineError("Malformed MESH chunk");
    return false;
  }  
  
  CHECK("AABB size is correct", sizeof(AxisAlignedBox) == 24);
  curr.read(&mBoundingBox, sizeof(AxisAlignedBox));

  curr.read(transparencyIndex);
  if(transparencyIndex != indexCount) {
    logEngineError("Transparency not supported");
  }
  mVertexData.resize(vertexCount);
  if(!curr.readCompressed(&mVertexData[0], sizeof(Vertex) * vertexCount)) {
    mVertexData.resize(0);
    return false;
  }
  mIndexData.resize(indexCount);
  if(!curr.readCompressed(&mIndexData[0], 4 * indexCount)) {
    mIndexData.resize(0);
    return false;
  }
  
  ++curr;
  if(curr != end && curr.getChunkType() == "COLL") {
    if(mCollisionSupport) {
      if(!mCollisionKDTree->load(curr, end, curr)) {
        flushCollision();
      }
    }
    else {
      ++curr;
    }
  }
  flushInternal();
  
  return true;
}


// --------------------------------------------------------------------------------

bool 
Mesh::save(ChunkStream* stream)
{
  stream->openChunk("MESH");
  int vertexCount = getVertexCount();
  int indexCount = getIndexCount();

  stream->write(vertexCount);
  stream->write(indexCount);

  CHECK("AABB size is correct", sizeof(AxisAlignedBox) == 24);
  stream->write(&mBoundingBox, sizeof(AxisAlignedBox));

  // Future support for transparent surfaces
  int transparencyIndex = indexCount;
  stream->write(transparencyIndex);

  CHECK("Vertex size is correct", sizeof(Vertex) == 36);
  stream->writeCompressedDeflate(&mVertexData[0], sizeof(Vertex) * vertexCount);
  stream->writeCompressedDeflate(&mIndexData[0], 4 * indexCount);
  stream->closeChunk();  

  if(mCollisionSupport)
    mCollisionKDTree->save(stream);
  return true;
}


// --------------------------------------------------------------------------------

void
Mesh::flush()
{
  mBoundingBox.clear();
  for(unsigned int iVertex = 0; iVertex< mVertexData.size(); ++iVertex) {
    mBoundingBox.addPoint(mVertexData[iVertex].position);
  }  
  flushCollision();
  flushInternal();
}


void
Mesh::flushCollision()
{
  if(mCollisionSupport) {
    KDTriList triangles;
    for(unsigned int iIndex = 0; iIndex < mIndexData.size() / 3; ++iIndex) {
      unsigned int baseIndex = iIndex * 3;
      KDTriangle triangle;
      const Vector& pos0 = mVertexData[mIndexData[baseIndex]].position;
      const Vector& pos1 = mVertexData[mIndexData[baseIndex + 1]].position;
      const Vector& pos2 = mVertexData[mIndexData[baseIndex + 2]].position;
      triangle.vert0 = pos0;
      triangle.edge1 = pos1 - pos0;
      triangle.edge2 = pos2 - pos0;
      triangle.index = iIndex;
      triangles.push_back(triangle);
    }
    mCollisionKDTree->createTree(mBoundingBox, triangles);
  }
}

// --------------------------------------------------------------------------------

Vector
Mesh::getMeshCenter()
{
  return (mBoundingBox.minPoint + mBoundingBox.maxPoint) * 0.5f;
}

float
Mesh::getBoundingRadius()
{
  return (mBoundingBox.minPoint - mBoundingBox.maxPoint).length() * 0.5f;
}

const AxisAlignedBox&
Mesh::getBoundingBox()
{
  return mBoundingBox;
}


// --------------------------------------------------------------------------------

bool
Mesh::intersectRayBox(const Ray& pickRay, Matrix transform, float& oDistance)
{
  if(!mCollisionSupport) return false;
  
  Matrix inverseTransform = transform.inverse();

  Ray localRay;
  localRay.origin = pickRay.origin * inverseTransform;
  inverseTransform.setTranslation(Vector::ZERO);
  localRay.direction = pickRay.direction * inverseTransform;
  
  float farDistance;
  return mBoundingBox.intersect(localRay, oDistance, farDistance);
}

// --------------------------------------------------------------------------------

bool
Mesh::intersectRayTri(const Ray& pickRay, Matrix transform, float& oDistance)
{
  if(!mCollisionSupport) return false;

  Vector pos1, pos2, pos3;
  Matrix inverseTransform = transform.inverse();
  Ray localRay;
  localRay.origin = pickRay.getOrigin() * inverseTransform;
  inverseTransform.setTranslation(Vector::ZERO);
  localRay.direction = pickRay.getDirection() * inverseTransform;
  localRay.direction.normalize();
  return mCollisionKDTree->intersectRay(localRay, oDistance);
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

bool
operator<(const ObjVertex& vertex1, const ObjVertex& vertex2)
{
  if(vertex1.iPos != vertex2.iPos) {
    return  vertex1.iPos < vertex2.iPos;
  }
  else if(vertex1.iTexture != vertex2.iTexture) {
    return vertex1.iTexture < vertex2.iTexture;
  }
  else {
    return vertex1.iNormal < vertex2.iNormal;
  }
}


void
Mesh::loadFromOBJ(string fileName)
{
  map<ObjVertex, int> vertexToIndex;
  vector<Vector> positions;
  vector<Vector> normals;
  vector<ObjTex> textures;
  
  mVertexData.clear();
  mIndexData.clear();
  const int MAX_LINE_SKIP = 1000;

  int line = 1;
  ifstream file(fileName.c_str());
  if(!file.is_open()) {
    logEngineError(string("Unable to open OBJ file: ") + fileName);
    return;
  }
  
  while(!file.eof() && !file.bad()) {
    char nextChar;
    nextChar = file.peek();

    if('#' == nextChar) {
      file.ignore(MAX_LINE_SKIP, '\n');
      line++;
      continue;
    }
    else if('\r' == nextChar || ' ' == nextChar || '\t' == nextChar || '\n' == nextChar) {
      file.get();
      continue;
    }

    string token;
    file >> token;
    if("v" == token) {
      Vector pos;
      file >> pos.x;
      file >> pos.y;
      file >> pos.z;
      pos.x = -pos.x;
      positions.push_back(pos);
    }
    else if("vn" == token) {
      Vector normal;
      file >> normal.x;
      file >> normal.y;
      file >> normal.z;
      normal.x = -normal.x;
      normals.push_back(normal);
    }
    else if("vt" == token) {
      float w;
      ObjTex tex;
      file >> tex.u;
      if(file >> tex.v)
        file >> w;
      else
        tex.v = 0;
      file.clear();      

      textures.push_back(tex);
    }
    else if("f" == token) {
      int count = 0;
      int vertices[3];
      ObjVertex objVertex;
      while(file >> objVertex.iPos) {
        if('/' == file.peek()) {
          file.ignore();
          if('/' == file.peek()) {
            objVertex.iTexture = -1;
          }
          else {
            file >> objVertex.iTexture;
          }
          
          if('/' != file.peek()) {
            ostringstream ossError;
            ossError << "OBJ Loader: " << fileName << "(" << line << ") : only 'f n ..' or 'f n/n/n ..' forms supported";
            logEngineWarn(ossError.str());
            break;
          }
          file.ignore();
          file >> objVertex.iNormal;
        }
        else {
          objVertex.iTexture = objVertex.iPos;
          objVertex.iNormal = objVertex.iPos;
        }
        int indexPlusOne = vertexToIndex[objVertex];
        if(0 == indexPlusOne) {
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
          
          mVertexData.push_back(vertex);
          indexPlusOne = mVertexData.size();
          vertexToIndex[objVertex] = indexPlusOne;
        }
        if(count >= 3) {
          vertices[1] = vertices[2];
          vertices[2] = indexPlusOne - 1;
        }
        else {
          vertices[count] = indexPlusOne - 1;
        }        
        if(count >= 2) {
          mIndexData.push_back(vertices[2]);
          mIndexData.push_back(vertices[1]);
          mIndexData.push_back(vertices[0]);
        }
        count++;
      }
      // This might not be enough to skip end of line? Are comments accepted anywhere?
      file.clear();
    }
    else {
      ostringstream ossErr;
      ossErr << "OBJ Loader: " << fileName << "(" << line << ") : Unknown token '" << token << "'";
      logEngineWarn(ossErr.str());
      file.ignore(MAX_LINE_SKIP, '\n');
    }
    line ++;
  }

  flush();
}


// --------------------------------------------------------------------------------
// Loading from an SSM Mesh data file


bool
operator<(const Vertex& lhs, const Vertex& rhs)
{
  if(lhs.position != rhs.position)
    return lhs.position < rhs.position;
  if(lhs.normal != rhs.normal)
    return lhs.normal < rhs.normal;

  if(lhs.u != rhs.u)
    return lhs.u < rhs.u;

  return lhs.v < rhs.v;
}



void
Mesh::loadFromSSM(string fileName)
{
  map<Vertex, int> vertexToIndex;
  
  const int MAX_LINE_SKIP = 1000;

//   int line = 1;
  ifstream file(fileName.c_str());
  mVertexData.clear();
  mIndexData.clear();
  
  int portNum;
  file >> portNum;
  while(file.peek() == '\n') file.ignore(MAX_LINE_SKIP, '\n');
  for(int skipPort = 0; skipPort < portNum; ++skipPort) {
    file.ignore(MAX_LINE_SKIP, '\n');
  }
  int materialNum;
  file >> materialNum;
  if(materialNum > 1) {
    logEngineWarn("SSM Mesh Loader: Too many materials in mesh file");
  }
  string materialName;
  while(file.peek() == '\n')
    file.ignore(MAX_LINE_SKIP, '\n'); // There is an odd empty line here..
  for(int skipMaterial = 0; skipMaterial < materialNum; ++skipMaterial)
    file.ignore(MAX_LINE_SKIP, '\n');

  int triangleCount;
  file >> triangleCount;
  int indexCount = triangleCount * 3;
  while(file.peek() == '\n') file.ignore(MAX_LINE_SKIP, '\n');
  for(int index = 0; index < indexCount; ++index) {
    Vertex vertex;
    file >> vertex.position.x >> vertex.position.y >> vertex.position.z
         >> vertex.normal.x >> vertex.normal.y >> vertex.normal.z
         >> vertex.u >> vertex.v;
    vertex.v = 1 - vertex.v;

    int indexPlusOne = vertexToIndex[vertex];
    if(0 == indexPlusOne) {
      mVertexData.push_back(vertex);
      indexPlusOne = mVertexData.size();
      vertexToIndex[vertex] = indexPlusOne;
    }
    mIndexData.push_back(indexPlusOne - 1);
    while(file.peek() == '\n') file.ignore(MAX_LINE_SKIP, '\n');
  }
  ostringstream ossLoadStat;
  ossLoadStat << "SSM Mesh Loader: " << mVertexData.size() << " vertices " << mIndexData.size() << " indices";
  logEngineInfo(ossLoadStat.str());

  flush();
}


// --------------------------------------------------------------------------------
// Loading simple unit cube

Vertex
loadCubeVertex(const Vector& right, const Vector& up, const Vector& normal, bool isRight, bool isUp, float scale)
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
Mesh::loadCubeSurface(const Vector& right, const Vector& up, const Vector& normal, float scale)
{
  Vertex ul = loadCubeVertex(right, up, normal, false, true, scale);
  Vertex ll = loadCubeVertex(right, up, normal, false, false, scale);
  Vertex lr = loadCubeVertex(right, up, normal, true, false, scale);
  Vertex ur = loadCubeVertex(right, up, normal, true, true, scale);

  ul.u = ll.u = 0;
  ur.u = lr.u = 1;
  ll.v = lr.v = 0;
  ul.v = ur.v = 1;
  
  int baseIndex = mVertexData.size();

  mVertexData.push_back(ul);
  mVertexData.push_back(ll);
  mVertexData.push_back(lr);
  mVertexData.push_back(ur);

  mIndexData.push_back(baseIndex + 2);
  mIndexData.push_back(baseIndex + 1);
  mIndexData.push_back(baseIndex + 0);

  mIndexData.push_back(baseIndex + 0);
  mIndexData.push_back(baseIndex + 3);
  mIndexData.push_back(baseIndex + 2);
}


void
Mesh::loadCube(float scale)
{
  scale = scale * 0.5;
  mVertexData.clear();
  mIndexData.clear();
  
  loadCubeSurface(Vector::UNIT_X, Vector::UNIT_Y, -Vector::UNIT_Z, scale);
  loadCubeSurface(-Vector::UNIT_X, Vector::UNIT_Y, Vector::UNIT_Z, scale);

  loadCubeSurface(Vector::UNIT_Z, Vector::UNIT_Y, Vector::UNIT_X, scale);
  loadCubeSurface(-Vector::UNIT_Z, Vector::UNIT_Y, -Vector::UNIT_X, scale);

  loadCubeSurface(Vector::UNIT_X, Vector::UNIT_Z, Vector::UNIT_Y, scale);
  loadCubeSurface(Vector::UNIT_X, -Vector::UNIT_Z, -Vector::UNIT_Y, scale);
  
  flush();
}
