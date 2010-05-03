//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_KDTREE_HPP
#define RENDERING_KDTREE_HPP

class ChunkStream;
class ChunkData;
class ChunkIterator;

struct KDTriangle
{
  Vector vert0;
  Vector edge1;
  Vector edge2;
  int index;
};

typedef vector<KDTriangle> KDTriList;

struct KDNode
{
  AxisAlignedBox mBounds;
  int mTriStart;
  int mTriNum;
  int mChildren[2];
};


class KDTree
{
public:
  KDTree();

  bool load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr);
  bool save(ChunkStream* chunkStream);

  void createTree(const AxisAlignedBox& bounds, const KDTriList& triangles);
  bool intersectRay(const Ray& localRay, float& oDistance);  
private:
  int createNode(const AxisAlignedBox& bounds);
  void splitNode(int nodeIndex, const KDTriList& triangles, int level);
  bool intersectRay(int nodeIndex, const Ray& localRay, float& oDistance);

  vector<KDNode> mNodes;
  KDTriList mTriangles;
};

#endif
