//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import std.math;
private import Ray;
private import Math;
private import Matrix;
private import Vector;

AxisAlignedBox
createAxisAlignedBox()
{
  AxisAlignedBox box;
  box.clear();
  return box;
}


struct AxisAlignedBox
{
public:
  static assert(AxisAlignedBox.sizeof == 24);

  bool intersect(Ray ray, out float oNear, out float oFar)
  {
    oNear = float.min;
    oFar = float.max;
    
    // Loop through the planes
    for(int iDimension = 0; iDimension < 3; ++iDimension) {
      if(fabs(ray.direction[iDimension]) < Math.ZERO_TOLERANCE &&
       (ray.origin[iDimension] < minPoint[iDimension] ||
        ray.origin[iDimension] > maxPoint[iDimension]))
        return false;
      float invDirection = 1.0f / ray.direction[iDimension];
      float t1 = (minPoint[iDimension] - ray.origin[iDimension]) * invDirection;
      float t2 = (maxPoint[iDimension] - ray.origin[iDimension]) * invDirection;
      if(t1 > t2) {
        float temp = t2; t2 = t1; t1 = temp;
      }
      if(t1 > oNear) oNear = t1;
      if(t2 < oFar) oFar = t2;
      if(oNear > oFar) return false;
      if(oFar < 0) return false;
    }
    return true;    
  }
  
  bool intersect(AxisAlignedBox rhs)
  {
    for(int iDimension = 0; iDimension < 3; ++iDimension) {
      if(maxPoint[iDimension] < rhs.minPoint[iDimension]) return false;
      if(minPoint[iDimension] > rhs.maxPoint[iDimension]) return false;
    }
    return true;
  }
  


  void clear()
  {
    for(int iDimension = 0; iDimension < 3; ++iDimension) {
      minPoint[iDimension] = float.max;
      maxPoint[iDimension] = float.min;
    }
  }
  
  void addPoint(Vector point)
  {
    for(int iDimension = 0; iDimension < 3; ++iDimension) {
      if(point.v[iDimension] < minPoint.v[iDimension]) minPoint.v[iDimension] = point.v[iDimension];
      if(point.v[iDimension] > maxPoint.v[iDimension]) maxPoint.v[iDimension] = point.v[iDimension];
    }
  }                 

  Matrix transform(float padding = 0)
  {
    Matrix shiftCenter = createMatrix((minPoint + maxPoint) * 0.5);
    Matrix scaleBox = Matrix.IDENTITY;
    scaleBox.setScaling(createVector(maxPoint.x - minPoint.x + padding,
                                     maxPoint.y - minPoint.y + padding,
                                     maxPoint.z - minPoint.z + padding));
    return scaleBox * shiftCenter;
  }
  
  
  Vector minPoint;
  Vector maxPoint;
};




  
  
