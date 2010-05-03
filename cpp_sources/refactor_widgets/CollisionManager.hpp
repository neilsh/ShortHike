//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef WIDGETS_COLLISION_MANAGER_HPP
#define WIDGETS_COLLISION_MANAGER_HPP

namespace ColDet 
{
  class CollisionModel3D;
}

class CollisionManager : public Singleton<CollisionManager>
{
public:
  CollisionManager();
  ~CollisionManager();
  
  void addMesh(string meshName, string groupName = RESOURCE_GROUP_MAIN);
  void addMesh(MeshPtr mesh);

  const AxisAlignedBox& getBoundingBox(string meshName, string groupName = RESOURCE_GROUP_MAIN);
  Real getBoundingRadius(string meshName, string groupName = RESOURCE_GROUP_MAIN);

  bool isIntersecting(const Ray& pickRay, Entity* entity, Real& distance);

private:
  map<string, ColDet::CollisionModel3D*> modelMap;
};


#endif
