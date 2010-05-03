//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef GAME_PART_HPP
#define GAME_PART_HPP

class Mesh;
class Texture;

class REFACTOR_Port
{
public:
  bool mMountPoint;
  Quaternion mOrientation;
  Vector mPosition;
};


// Each module has a single mesh, Transaparent surfaces are ordered at the end.


class Part
{
public:
  Mesh* mMesh;
  Texture* mTexture;
  
  vector<REFACTOR_Port> mPorts;
};

#endif
