//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"
#include "KDTree.hpp"

#include "file/ChunkData.hpp"
#include "file/ChunkStream.hpp"

// --------------------------------------------------------------------------------

const unsigned int MIN_KDTREE_TRIANGLES = 5;
const int MAX_KDTREE_LEVEL = 10;
const unsigned int KDTREE_CHILDREN = 2;

KDTree::KDTree()
{
}


// --------------------------------------------------------------------------------

bool
KDTree::load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr)
{
  CHECK("Valid data sizes", sizeof(KDNode) == 40 && sizeof(KDTriangle) == 40);
  curr = begin;
  if(curr == end) return false;
  
  if(curr.getChunkType() != "COLL") return false;

  size_t nodeNum;
  size_t triangleNum;
  curr.read(nodeNum);
  curr.read(triangleNum);
    
  mNodes.clear();
  mNodes.resize(nodeNum);
  curr.readCompressed(&mNodes[0], sizeof(KDNode) * nodeNum);
  
  mTriangles.clear();
  mTriangles.resize(triangleNum);
  curr.readCompressed(&mTriangles[0], sizeof(KDTriangle) * triangleNum);

  ++curr;
  return true;
}

// --------------------------------------------------------------------------------

bool
KDTree::save(ChunkStream* stream)
{
  CHECK("Valid data sizes", sizeof(KDNode) == 40 && sizeof(KDTriangle) == 40);

  size_t nodeNum = mNodes.size();
  size_t triangleNum = mTriangles.size();

  stream->openChunk("COLL");
  stream->write(nodeNum);
  stream->write(triangleNum);
  stream->writeCompressedDeflate(&mNodes[0], sizeof(KDNode) * nodeNum);
  stream->writeCompressedDeflate(&mTriangles[0], sizeof(KDTriangle) * triangleNum);
  stream->closeChunk();  

  return true;
}


// --------------------------------------------------------------------------------

void
KDTree::createTree(const AxisAlignedBox& initialBounds, const KDTriList& sourceTri)
{
  mNodes.clear();
  mNodes.reserve(sourceTri.size() / MIN_KDTREE_TRIANGLES);
  mTriangles.clear();
  mTriangles.reserve(sourceTri.size());

  splitNode(createNode(initialBounds), sourceTri, 0);
}

bool
KDTree::intersectRay(const Ray& localRay, float& oDistance)
{
  if(mNodes.size() < 1) return false;
  return intersectRay(0, localRay, oDistance);
}

// --------------------------------------------------------------------------------

int
KDTree::createNode(const AxisAlignedBox& initialBounds)
{
  KDNode newNode;
  newNode.mBounds = initialBounds;
  newNode.mTriStart = 0;
  newNode.mTriNum = 0;
  newNode.mChildren[0] = 0;
  newNode.mChildren[1] = 0;
  mNodes.push_back(newNode);
  return mNodes.size() - 1;
}


// --------------------------------------------------------------------------------

#pragma warning(disable: 4100)

void
KDTree::splitNode(int nodeIndex, const KDTriList& sourceTri, int level)
{
  AxisAlignedBox triBox;
  KDTriList myTriangles;
  for(KDTriList::const_iterator iTriangle = sourceTri.begin(); iTriangle != sourceTri.end(); ++iTriangle) {
    KDTriangle triangle = *iTriangle;
    triBox.clear();
    triBox.addPoint(triangle.vert0);
    triBox.addPoint(triangle.vert0 + triangle.edge1);
    triBox.addPoint(triangle.vert0 + triangle.edge2);
    if(mNodes[nodeIndex].mBounds.intersect(triBox))
      myTriangles.push_back(triangle);
  }
  // Split ends here
  if(myTriangles.size() < MIN_KDTREE_TRIANGLES || level == MAX_KDTREE_LEVEL) {
    for(int iChild = 0; iChild < KDTREE_CHILDREN; ++iChild)
      mNodes[nodeIndex].mChildren[iChild] = 0;
    mNodes[nodeIndex].mTriStart = mTriangles.size();
    mNodes[nodeIndex].mTriNum = myTriangles.size();
    mTriangles.resize(mTriangles.size() + myTriangles.size());
    mTriangles.insert(mTriangles.begin() + mNodes[nodeIndex].mTriStart, myTriangles.begin(), myTriangles.end());
  }
  // No triangles stored, split children instead
  else {
    for(int iChild = 0; iChild < KDTREE_CHILDREN; ++iChild) {
      AxisAlignedBox childBounds;
      int splitDimension = level % 3;
      for(int iDimension = 0; iDimension < 3; ++iDimension) {
        if(iDimension == splitDimension) {
          if(iChild  & 0x1) {
            childBounds.minPoint[iDimension] = mNodes[nodeIndex].mBounds.minPoint[iDimension];
            childBounds.maxPoint[iDimension] = (mNodes[nodeIndex].mBounds.minPoint[iDimension] + mNodes[nodeIndex].mBounds.maxPoint[iDimension]) * 0.5f;
          }
          else {
            childBounds.minPoint[iDimension] = (mNodes[nodeIndex].mBounds.minPoint[iDimension] + mNodes[nodeIndex].mBounds.maxPoint[iDimension]) * 0.5f;
            childBounds.maxPoint[iDimension] = mNodes[nodeIndex].mBounds.maxPoint[iDimension];
          }
        }
        else {
          childBounds.minPoint[iDimension] = mNodes[nodeIndex].mBounds.minPoint[iDimension];
          childBounds.maxPoint[iDimension] = mNodes[nodeIndex].mBounds.maxPoint[iDimension];
        }
      }
      mNodes[nodeIndex].mChildren[iChild] = createNode(childBounds);
      splitNode(mNodes[nodeIndex].mChildren[iChild], myTriangles, level + 1);
    }
  }
}


// --------------------------------------------------------------------------------
// Triangle collision code pulled from: http://www.cs.lth.se/home/Tomas_Akenine_Moller/code/

// the original jgt code
inline bool
intersect_triangle(const Ray& ray, const KDTriangle& triangle,
                   float& oDistance)
{
  const float EPSILON = 0.000001f;
  
  // begin calculating determinant - also used to calculate U parameter
  Vector pvec = ray.direction.crossProduct(triangle.edge2);

  // if determinant is near zero, ray lies in plane of triangle
  float det = triangle.edge1.dotProduct(pvec);

  // CULLING version
  if (det < EPSILON)
    return false;

  // calculate distance from vert0 to ray origin
  Vector tvec = ray.origin - triangle.vert0;

  // calculate U parameter and test bounds
  float u = tvec.dotProduct(pvec);
  if (u < 0.0 || u > det)
    return false;

  // prepare to test V parameter
  Vector qvec = tvec.crossProduct(triangle.edge1);

  // calculate V parameter and test bounds
  float v = ray.direction.dotProduct(qvec);
  if (v < 0.0 || u + v > det)
    return false;

  // calculate t, scale parameters, ray intersects triangle
  oDistance = triangle.edge2.dotProduct(qvec);
  float inv_det = 1.0 / det;
  oDistance *= inv_det;
//   u *= inv_det;
//   v *= inv_det;
  
  return true;
}


// --------------------------------------------------------------------------------

bool
KDTree::intersectRay(int nodeIndex, const Ray& localRay, float& oDistance)
{
  float boxNear, boxFar;
  if(!mNodes[nodeIndex].mBounds.intersect(localRay, boxNear, boxFar)) return false;
  
  float distance;
  for(int iTri = mNodes[nodeIndex].mTriStart; iTri < (mNodes[nodeIndex].mTriStart + mNodes[nodeIndex].mTriNum); ++iTri) {
    KDTriangle& triangle = mTriangles[iTri];
    if(intersect_triangle(localRay, triangle, distance)) {
      oDistance = distance;
      return true;
    }
  }
    
  for(unsigned int iChild = 0; iChild < KDTREE_CHILDREN; ++iChild) {
    if(mNodes[nodeIndex].mChildren[iChild] != 0 && intersectRay(mNodes[nodeIndex].mChildren[iChild], localRay, oDistance))
      return true;
  }  
  
  return false;
}
