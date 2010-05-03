//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#ifndef USER_INTERFACE_ORBIT_PLANET_MESH_HPP
#define USER_INTERFACE_ORBIT_PLANET_MESH_HPP

class TexturePatch;

class PlanetMesh
{
  friend ostream& operator<<(ostream& out, PlanetMesh*);
public:
  enum State {
    LOADING_TEXTURE,
    UPDATING_MESH,
    READY
  };  

  PlanetMesh(SceneManager* sceneManager, string meshName, string diffuseMap, string topoMap, Real radius, Camera* defaultCamera, int gridSize = 4);
  ~PlanetMesh();

  void startLoad(Real generateAhead);
  State loadIncremental();
  void loadAll(bool reportProgress = false);

  void setObserverPosition(Degree inclination, Degree ascending, Real radius, Degree angle);
  void synchronizeCamera();

  void setVisible(bool isVisible) { planetEntity->setVisible(isVisible);}  

  bool isWithinEffectiveBounds();
  bool movingAway();
  Degree getAngle(const Vector& observerPosition);

  Degree getLongitude();
  Degree getLatitude();
  Degree getLongitude(Vector& observerPosition);
  Degree getLatitude(Vector& observerPosition);
private:
  Quaternion getPlanetOrientation() const;
  Quaternion getObserverOrientation() const;
  void createMeshGrid(string fullName);
  void updatePlanetMesh();

  SubMesh* getSubMesh(int xpos, int ypos);
  SubEntity* getSubEntity(int xpos, int ypos);
  Sphere& getSubMeshBounds(int xpos, int ypos);  

  int gridSize;

  State state;
  
  Vector observerPosition;
  Vector orbitalAxis;
  
  Degree orbitInclination;
  Degree orbitAscending;
  Real orbitRadius;
  Degree orbitAngle;

  Vector meshPosition;
  Camera* camera;
  Real radius;
  Entity* planetEntity;
  
  Real linearCullDistance;
  Degree northLatitude;
  Degree southLatitude;
  Degree westLongitude;
  Degree eastLongitude;

  Degree gridLatitudeStep;
  Degree gridLongitudeStep;

  Degree effectiveAngle;

  MeshPtr mesh;
  
  SceneNode* orbitAnchor;

  TexturePatch* diffuseTexturePatch;

  vector<SubMesh*> subMeshes;
  vector<Sphere> subMeshBounds;

  static int instanceCounter;
};


ostream& operator<<(ostream& out, PlanetMesh*);

#endif
