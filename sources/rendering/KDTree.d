//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import ChunkData;
private import ChunkStream;


struct KDTriangle
{
  Vector vert0;
  Vector edge1;
  Vector edge2;
  int index;
}

typedef KDTriangle[] KDTriList;

struct KDNode
{
  AxisAlignedBox mBounds;
  int mTriStart;
  int mTriNum;
  int mChildren[2];
};


// --------------------------------------------------------------------------------

const int MIN_KDTREE_TRIANGLES = 5;
const int MAX_KDTREE_LEVEL = 10;
const int KDTREE_CHILDREN = 2;

static assert(KDNode.init.sizeof == 40 && KDTriangle.init.sizeof == 40);

class KDTree
{
public:

  this()
  {
  }
  
  
  // --------------------------------------------------------------------------------

  static assert(KDNode.sizeof == 40);
  static assert(KDTriangle.sizeof == 40);

  
  bool
  load(ChunkIterator begin, ChunkIterator end, out ChunkIterator curr)
  {
    curr = begin;
    if(curr == end) return false;
    
    if(curr.getChunkType() != "COLL") {
      logError << "KDTree chunk type error: " << curr.getChunkType << endl;
      return false;
    }
    
    uint nodeNum;
    uint triangleNum;
    curr.read(nodeNum);
    curr.read(triangleNum);
    
    mNodes.length = nodeNum;
    ubyte[] nodeTemp = cast(ubyte[])mNodes;
    curr.readCompressed(nodeTemp);
    
    mTriangles.length = triangleNum;
    ubyte[] triTemp = cast(ubyte[])mTriangles;
    curr.readCompressed(triTemp);
    
    curr++;
    return true;
  }

// --------------------------------------------------------------------------------

bool
save(ChunkStream stream)
{

  uint nodeNum = mNodes.length;
  uint triangleNum = mTriangles.length;

  stream.openChunk("COLL");
  stream.write(nodeNum);
  stream.write(triangleNum);
  stream.writeCompressedDeflate(&mNodes[0], KDNode.init.sizeof * nodeNum);
  stream.writeCompressedDeflate(&mTriangles[0], KDTriangle.init.sizeof * triangleNum);
  stream.closeChunk();  

  return true;
}


// --------------------------------------------------------------------------------

  void
  createTree(AxisAlignedBox initialBounds, KDTriList sourceTri)
  {
    mNodes.length = 0;
//     mNodes.reserve(sourceTri.size() / MIN_KDTREE_TRIANGLES);
    mTriangles.length = 0;
//     mTriangles.reserve(sourceTri.size);
    
    splitNode(createNode(initialBounds), sourceTri, 0);
  }
  
  bool
  intersectRay(Ray localRay, out float oDistance)
  {
    if(mNodes.length < 1) return false;
    return intersectRay(0, localRay, oDistance);
  }

// --------------------------------------------------------------------------------

  int
  createNode(AxisAlignedBox initialBounds)
  {
    KDNode newNode;
    newNode.mBounds = initialBounds;
    newNode.mTriStart = 0;
    newNode.mTriNum = 0;
    newNode.mChildren[0] = 0;
    newNode.mChildren[1] = 0;
    mNodes ~= newNode;
    return mNodes.length - 1;
  }


  // --------------------------------------------------------------------------------
  

  void
  splitNode(int nodeIndex, KDTriList sourceTri, int level)
  {
    AxisAlignedBox triBox;
    KDTriList myTriangles;
    for(int iTriangle = 0; iTriangle < sourceTri.length; ++iTriangle) {
      KDTriangle triangle = sourceTri[iTriangle];
      triBox.clear();
      triBox.addPoint(triangle.vert0);
      triBox.addPoint(triangle.vert0 + triangle.edge1);
      triBox.addPoint(triangle.vert0 + triangle.edge2);
      if(mNodes[nodeIndex].mBounds.intersect(triBox))
        myTriangles ~= triangle;
    }
    // Split ends here
    if(myTriangles.length < MIN_KDTREE_TRIANGLES || level == MAX_KDTREE_LEVEL) {
      for(int iChild = 0; iChild < KDTREE_CHILDREN; ++iChild)
        mNodes[nodeIndex].mChildren[iChild] = 0;
      mNodes[nodeIndex].mTriStart = mTriangles.length;
      mNodes[nodeIndex].mTriNum = myTriangles.length;
      mTriangles ~= myTriangles;
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
  bool
  intersect_triangle(Ray ray, KDTriangle triangle, out float oDistance)
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
  intersectRay(int nodeIndex, Ray localRay, out float oDistance)
  {
    float boxNear, boxFar;
    if(!mNodes[nodeIndex].mBounds.intersect(localRay, boxNear, boxFar)) return false;
    
    float distance;
    for(int iTri = mNodes[nodeIndex].mTriStart; iTri < (mNodes[nodeIndex].mTriStart + mNodes[nodeIndex].mTriNum); ++iTri) {
      KDTriangle triangle = mTriangles[iTri];
      if(intersect_triangle(localRay, triangle, distance)) {
        oDistance = distance;
        return true;
      }
    }
    
    for(int iChild = 0; iChild < KDTREE_CHILDREN; ++iChild) {
      if(mNodes[nodeIndex].mChildren[iChild] != 0 && intersectRay(mNodes[nodeIndex].mChildren[iChild], localRay, oDistance))
        return true;
    }  
    
    return false;
  }
  
private:
  KDNode[] mNodes;
  KDTriList mTriangles;
};

