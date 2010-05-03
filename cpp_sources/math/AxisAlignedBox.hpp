//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef MATH_AXIS_ALIGNED_BOX_HPP
#define MATH_AXIS_ALIGNED_BOX_HPP

class AxisAlignedBox
{
public:
  AxisAlignedBox()
  {
    clear();
  }

  inline bool intersect(const Ray& ray, float& oNear, float& oFar) const
  {
    oNear = FLT_MIN;
    oFar = FLT_MAX;
    
    // Loop through the planes
    for(int iDimension = 0; iDimension < 3; ++iDimension) {
      if(fabs(ray.direction[iDimension]) < Math::ZERO_TOLERANCE &&
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
  
  inline bool intersect(const AxisAlignedBox& rhs) const
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
      minPoint[iDimension] = FLT_MAX;
      maxPoint[iDimension] = FLT_MIN;
    }
  }
  
  void addPoint(Vector point)
  {
    for(int iDimension = 0; iDimension < 3; ++iDimension) {
      if(point.v[iDimension] < minPoint.v[iDimension]) minPoint.v[iDimension] = point.v[iDimension];
      if(point.v[iDimension] > maxPoint.v[iDimension]) maxPoint.v[iDimension] = point.v[iDimension];
    }
  }                 
  
  Vector minPoint;
  Vector maxPoint;
};


#endif
