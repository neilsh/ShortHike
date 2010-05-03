//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Vector;

Ray cRay(Vector origin, Vector direction)
{
  Ray* result = new Ray;
  result.origin = origin;
  result.direction = direction;
  return *result;
}

struct Ray
{
public:  
  Vector getOrigin()
  {
    return origin;
  }
  
  Vector getDirection() 
  {
    return direction;
  }

  Vector origin;
  Vector direction;
};

