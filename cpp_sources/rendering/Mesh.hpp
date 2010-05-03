//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_MESH_HPP
#define RENDERING_MESH_HPP

class ChunkStream;
class ChunkData;
class ChunkIterator;

class DX7RenderSystem;
class KDTree;

#pragma warning(disable: 4201)
class Vertex
{
public:
  Vertex() : u(0), v(0), anim_info(0) {}  

  Vector position;
  Vector normal;
  float u, v;
  union 
  {
    struct 
    {
      int8u primary_bone, primary_weight, secondary_bone, secondary_weight;
    };
    unsigned int anim_info;
  };  
};

typedef vector<Vertex> VertexData;
typedef vector<unsigned int> IndexData;

class Mesh
{
  friend class DX7RenderSystem;
public:
  virtual ~Mesh();

  bool load(string fileName);
  bool load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr);
  bool save(ChunkStream* chunkStream);

  void loadFromOBJ(string fileName);
  void loadFromSSM(string fileName);
  void loadCube(float scale = 1);

  void flush();

  bool intersectRayBox(const Ray& pickRay, Matrix world, float& oDistance);
  bool intersectRayTri(const Ray& pickRay, Matrix world, float& oDistance);
  
  VertexData& getVertexData() {return mVertexData;}
  IndexData& getIndexData() {return mIndexData;}
    
  int getVertexCount() const {return mVertexData.size();}
  int getIndexCount() const {return mIndexData.size();}

  Vector getMeshCenter();
  float getBoundingRadius();
  const AxisAlignedBox& getBoundingBox();
protected:
  Mesh(bool collisionSupport);

  void loadCubeSurface(const Vector& right, const Vector& up, const Vector& normal, float scale);

  void flushCollision();
  virtual void flushInternal() = 0;

  VertexData mVertexData;
  IndexData mIndexData;

  AxisAlignedBox mBoundingBox;

  bool mCollisionSupport;
  KDTree* mCollisionKDTree;
};


#endif
