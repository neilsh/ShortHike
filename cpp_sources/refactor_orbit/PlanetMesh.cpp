//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "PlanetMesh.hpp"
#include "TexturePatch.hpp"
#include "screens/LoadingScreen.hpp"

const unsigned short HW_BUFFER_BINDING = 0;
const RenderQueueGroupID RENDER_QUEUE_PLANET = RENDER_QUEUE_2;

// The lower the grid size, the higher the subdivision needs to be
// const int GRID_SIZE = 4;
const int GRID_SUBDIVISION = 10;
const int GRID_PATCH_VERTICES = GRID_SUBDIVISION + 1;

const float AROUND_HORIZON = 1.5;
const float EFFECTIVE_ANGLE_COEFFICIENT = 0.25;
const float LINEAR_CULL_EXTENSION = 1.7;

//----------------------------------------
// Overdraw is used to fade away the seams between
// teture tiles. By overdrawing slightly and applying
// an alpha map we can distort the seams completely.

const float GRID_OVERDRAW = 0.05;

int PlanetMesh::instanceCounter = 0;

//--------------------------------------------------------------------------------

PlanetMesh::PlanetMesh(SceneManager* sceneManager, string meshName, string diffuseMap, string topoMap, float _radius, Camera* defaultCamera,
                       int _gridSize)
  : radius(_radius), camera(defaultCamera), gridSize(_gridSize)
{
  ostringstream fullName;
  fullName << "PlanetMesh_" << meshName << "_" << diffuseMap << instanceCounter++;
  mesh = MeshManager::getSingleton().createManual(fullName.str(), RESOURCE_GROUP_MAIN);

  diffuseTexturePatch = new TexturePatch(diffuseMap, fullName.str(), GRID_OVERDRAW, gridSize, 3);

  createMeshGrid(fullName.str());

  orbitAnchor = sceneManager->createSceneNode(fullName.str() + "_orbitAnchor");
  
  planetEntity = sceneManager->createEntity(fullName.str() + "_entity" , fullName.str());
  planetEntity->setRenderQueueGroup(RENDER_QUEUE_PLANET);
  orbitAnchor->attachObject(planetEntity);
  sceneManager->getRootSceneNode()->addChild(orbitAnchor);
}

PlanetMesh::~PlanetMesh()
{
  orbitAnchor->detachAllObjects();
  orbitAnchor->getCreator()->removeEntity(planetEntity);
  orbitAnchor->getParent()->removeChild(orbitAnchor);
  delete orbitAnchor;
  
  delete diffuseTexturePatch;
  MeshManager::getSingleton().unload(mesh->getName());
  mesh.setNull();
}


//--------------------------------------------------------------------------------

void
PlanetMesh::createMeshGrid(string fullName)
{
  // We need to loop on xposition in the inner loop to preserve indices for accessing
  // SubMesh/SubEntities later.
  for(int ypos = 0; ypos < gridSize; ++ypos) {
    for(int xpos = 0; xpos < gridSize; ++xpos) {
      LoadingScreen::tick();
      ostringstream subMeshName;
      subMeshName << xpos << "_" << ypos;
      SubMesh* subMesh = mesh->createSubMesh(subMeshName.str());
      subMesh->useSharedVertices = false;

      subMesh->vertexData = new VertexData();
      
      {
        // Create vertex declaration for our geometry
        VertexDeclaration* vertexDeclaration = subMesh->vertexData->vertexDeclaration;
        size_t offset = 0;

        vertexDeclaration->addElement(HW_BUFFER_BINDING, 0, VET_FLOAT3, VES_POSITION);
        offset += VertexElement::getTypeSize(VET_FLOAT3);
         
        vertexDeclaration->addElement(HW_BUFFER_BINDING, offset, VET_FLOAT3, VES_NORMAL);
        offset += VertexElement::getTypeSize(VET_FLOAT3);
        
        // Texture coordinates
        vertexDeclaration->addElement(HW_BUFFER_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
        offset += VertexElement::getTypeSize(VET_FLOAT2);
        
        // Tangent space vectors
        vertexDeclaration->addElement(HW_BUFFER_BINDING, offset, VET_FLOAT3, VES_TEXTURE_COORDINATES, 1);
      }
      
      
      // Set size of vertices used
      int vertexCount = GRID_PATCH_VERTICES * GRID_PATCH_VERTICES;      
      subMesh->vertexData->vertexStart = 0;
      subMesh->vertexData->vertexCount = vertexCount;

      // Create indexbuffer
      HardwareBufferManager& hwManager = HardwareBufferManager::getSingleton();
      int triangleCount = GRID_SUBDIVISION * GRID_SUBDIVISION * 2;
      HardwareIndexBufferSharedPtr indexBuffer =
        hwManager.createIndexBuffer(HardwareIndexBuffer::IT_32BIT, triangleCount * 3, HardwareBuffer::HBU_STATIC);

      subMesh->indexData = new IndexData();
      subMesh->indexData->indexBuffer = indexBuffer;

      // Triangle creation
      long* indexData = static_cast<long*>(indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
      for(int Yindex = 0; Yindex < GRID_SUBDIVISION; ++Yindex) {
        for(int Xindex = 0; Xindex < GRID_SUBDIVISION; ++Xindex) {
          long leftTop = Xindex + Yindex * GRID_PATCH_VERTICES;
          long rightTop = (Xindex + 1) + Yindex * GRID_PATCH_VERTICES;
          long leftBottom = Xindex + (Yindex + 1) * GRID_PATCH_VERTICES;
          long rightBottom = (Xindex + 1) + (Yindex + 1) * GRID_PATCH_VERTICES;
          
          *indexData++ = leftBottom;
          *indexData++ = rightTop;
          *indexData++ = leftTop;
          
          *indexData++ = rightTop;
          *indexData++ = leftBottom;
          *indexData++ = rightBottom;
        }    
      }
      indexBuffer->unlock();
      
      int triangleVertexCount = triangleCount * 3;
      subMesh->indexData->indexStart = 0;
      subMesh->indexData->indexCount = triangleVertexCount;
      
      // Clone the base material and apply diffuse texture to Tech0/Pass0/Unit1

      MaterialPtr baseMaterial = static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName("site_planet_ground"));
      ostringstream materialName;
      materialName << fullName << "_" << xpos << "_" << ypos;
      MaterialPtr cloneMaterial = baseMaterial->clone(materialName.str());
      subMesh->setMaterialName(materialName.str());

      TextureUnitState* textureUnit = cloneMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(1);
      textureUnit->setTextureName(diffuseTexturePatch->getTexture(xpos, ypos)->getName());

      subMeshes.push_back(subMesh);
      subMeshBounds.push_back(Sphere());
    }
  }
}


//--------------------------------------------------------------------------------


void
PlanetMesh::synchronizeCamera()
{
  // DEBUG
//   Vector debugDistancePosition = observerPosition * 1.5;
//   orbitAnchor->setPosition(camera->getWorldPosition() - debugDistancePosition);
  orbitAnchor->setPosition(camera->getWorldPosition() - Vector(0, 0, orbitRadius));
  orbitAnchor->setOrientation(getPlanetOrientation());
}

//--------------------------------------------------------------------------------

Quaternion
PlanetMesh::getPlanetOrientation() const
{
  Quaternion ascendingRotation = Quaternion(orbitAscending, Vector::UNIT_Y);
  Quaternion inclinationRotation = Quaternion(orbitInclination, Vector::UNIT_Z);
  Quaternion angleRotation = Quaternion(orbitAngle * -1, Vector::UNIT_Y);
  Quaternion orientation = angleRotation * inclinationRotation * ascendingRotation;
  orientation.normalise();
  return orientation;
}

Quaternion
PlanetMesh::getObserverOrientation() const
{
  Quaternion ascendingRotation = Quaternion(orbitAscending * -1, Vector::UNIT_Y);
  Quaternion inclinationRotation = Quaternion(orbitInclination * -1, Vector::UNIT_Z);
  Quaternion angleRotation = Quaternion(orbitAngle, Vector::UNIT_Y);
  Quaternion orientation = ascendingRotation * inclinationRotation * angleRotation;
  orientation.normalise();
  return orientation;
}

  
//--------------------------------------------------------------------------------
// Set the observer position


void
PlanetMesh::setObserverPosition(Degree _inclination, Degree _ascending, Real _radius, Degree _angle)
{
  orbitInclination = _inclination;
  orbitAscending = _ascending;
  orbitRadius = _radius;
  orbitAngle = _angle;
  
  observerPosition = Vector(0, 0, orbitRadius);
  orbitalAxis = Vector::UNIT_Y;
  Quaternion observerOrientation = getObserverOrientation();
  observerPosition = observerOrientation * observerPosition;
  orbitalAxis = observerOrientation * orbitalAxis;
  orbitalAxis.normalise();

  for(int xpos = 0; xpos < gridSize; ++xpos) {
    for(int ypos = 0; ypos < gridSize; ++ypos) {
      SubEntity* subEntity = getSubEntity(xpos, ypos);
      Sphere& bounds = getSubMeshBounds(xpos, ypos);
      Vector difference = observerPosition - bounds.getCenter();
      if((difference.length() + bounds.getRadius()) > linearCullDistance) {
        subEntity->setVisible(false);
      }
      else {
        subEntity->setVisible(true);
      }
    }
  }

  synchronizeCamera();
}


//--------------------------------------------------------------------------------
// Start the loading process
//
// Warning! There are a lot of dependencies in this calculation. This order results
// in a correct set of values in all situations, including around the singularities
// near the poles. Read carefully before you start to move anything around!

void
PlanetMesh::startLoad(Real generateAhead)
{
  float distanceFromCenter = observerPosition.length();
  Degree viewAngle = Radian(acos(radius / distanceFromCenter)) * 2;

  // Calculate the effective mesh position using orbitalAxis and generateAhead
  // The effective angle is the angle between the observer and the mesh_center
  effectiveAngle = viewAngle * EFFECTIVE_ANGLE_COEFFICIENT * 0.5;
  meshPosition = Quaternion(Radian(effectiveAngle.valueRadians() * generateAhead * 2), orbitalAxis) * observerPosition;
  
  Degree focusLatitude = getLatitude(meshPosition);
  Degree focusLongitude = getLongitude(meshPosition);

  Degree latitudeViewAngle = viewAngle * AROUND_HORIZON;
  
  northLatitude = focusLatitude + (latitudeViewAngle * 0.5);
  southLatitude = focusLatitude - (latitudeViewAngle * 0.5);

  // Correct for overwrap at the pole
  if(northLatitude.valueDegrees() > 90) {
    southLatitude = southLatitude.valueDegrees() - (northLatitude.valueDegrees() - 90);
    northLatitude = 90;
  }
  if(southLatitude.valueDegrees() < -90) {
    northLatitude = northLatitude.valueDegrees() + (-southLatitude.valueDegrees() - 90);
    southLatitude = -90;
  }
  
  // The longitudeViewAngle is extended max 360 degrees around the globe (at the poles)
  // Start by testing for singularity (leading to division by zero) at the pole
  Degree longitudeViewAngle;
  if(abs(focusLatitude.valueDegrees()) > 89.5) {
    longitudeViewAngle = 360;
  }
  else {
    longitudeViewAngle = abs(latitudeViewAngle.valueDegrees() / cos(focusLatitude.valueRadians()));
    if(longitudeViewAngle.valueDegrees() > 360) longitudeViewAngle = 360;
  }
  
  westLongitude = focusLongitude - (longitudeViewAngle * 0.5);
  eastLongitude = westLongitude + longitudeViewAngle;

  // After using this for longitudeViewAngle calculations correct it to the real value
  latitudeViewAngle = northLatitude - southLatitude;  

  // Set the longitude and latitude steps used in the mesh calculations
  gridLatitudeStep = -latitudeViewAngle / gridSize;
  gridLongitudeStep = longitudeViewAngle / gridSize;

  // Convert the latitude and longitude into some real coordinates
  float left = (westLongitude.valueDegrees() + 180.0f) / 360.0f;
  float right = (eastLongitude.valueDegrees() + 180.0f) / 360.0f;
  float top = 1.0f - (northLatitude.valueDegrees() + 90.0f) / 180.0f;
  float bottom = 1.0f - (southLatitude.valueDegrees() + 90.0f) / 180.0f;  

  // Solving linear culling distance
  float halfVisibleAngle = Degree(latitudeViewAngle * 0.5).valueRadians();
  float xleg = sin(halfVisibleAngle) * radius;
  float yleg = radius - cos(halfVisibleAngle) * radius;
  linearCullDistance = sqrt (xleg * xleg + yleg * yleg) * LINEAR_CULL_EXTENSION;

  diffuseTexturePatch->startLoad(left, right, top, bottom);

  state = LOADING_TEXTURE;
}


PlanetMesh::State
PlanetMesh::loadIncremental()
{
  if(state == LOADING_TEXTURE) {
    if(diffuseTexturePatch->loadIncremental() == TexturePatch::ALL_READY) {
      state = UPDATING_MESH;
    }
  }
  else if(state == UPDATING_MESH) {
    updatePlanetMesh();
    state = READY;
  }
  
  return state;
}


void
PlanetMesh::loadAll(bool reportProgress)
{
  if(reportProgress)
    while(loadIncremental() != READY) LoadingScreen::tick();
  else
    while(loadIncremental() != READY);
}


//--------------------------------------------------------------------------------

SubMesh*
PlanetMesh::getSubMesh(int xpos, int ypos)
{
  return subMeshes[xpos + ypos * gridSize];
}

SubEntity*
PlanetMesh::getSubEntity(int xpos, int ypos) 
{
  return planetEntity->getSubEntity(xpos + ypos * gridSize);
}


Sphere&
PlanetMesh::getSubMeshBounds(int xpos, int ypos)
{
  return subMeshBounds[xpos + ypos * gridSize];
}



//--------------------------------------------------------------------------------
// Update planet mesh
// Load the new planet detail information into the planet detail mesh
// Topo dataset generated with levels 126-162 in PhotoShop
// POSITIVE Y Points at North Pole
// POSITIVE Z Points at 0 Lon 0 Lat
// POSITIVE X Points at 90 E Lon 0 Lat

void
PlanetMesh::updatePlanetMesh()
{
  AxisAlignedBox meshBounds;

  HardwareBufferManager& hwManager = HardwareBufferManager::getSingleton();

  for(int subx = 0; subx < gridSize; ++subx) {
    for(int suby = 0; suby < gridSize; ++suby) {
      SubMesh* subMesh = getSubMesh(subx, suby);
      AxisAlignedBox subMeshBounds;

      // Create new vertex buffer for each write
      int vertexCount = GRID_PATCH_VERTICES * GRID_PATCH_VERTICES;      
      VertexDeclaration* vertexDeclaration = subMesh->vertexData->vertexDeclaration;
      HardwareVertexBufferSharedPtr hwBuffer = hwManager.createVertexBuffer(vertexDeclaration->getVertexSize(HW_BUFFER_BINDING), 
                                                                            vertexCount, HardwareBuffer::HBU_DYNAMIC);
      VertexBufferBinding* vertexBinding = subMesh->vertexData->vertexBufferBinding;
      vertexBinding->setBinding(HW_BUFFER_BINDING, hwBuffer);

      // Calculate the lat/lon information
      Degree startLatitude = northLatitude + static_cast<Real>(suby) * gridLatitudeStep - gridLatitudeStep * GRID_OVERDRAW * 0.5;
      Degree latitudeStep = gridLatitudeStep * (1.0 + GRID_OVERDRAW) / GRID_SUBDIVISION;

      Degree startLongitude = westLongitude + static_cast<Real>(subx) * gridLongitudeStep - gridLongitudeStep * GRID_OVERDRAW * 0.5;
      Degree longitudeStep = gridLongitudeStep * (1.0 + GRID_OVERDRAW) / GRID_SUBDIVISION;


      Degree longitude = startLongitude;
      Degree latitude = startLatitude;
      
      // Fill in the actual vertex information
      float* vertexData = static_cast<float*>(hwBuffer->lock(HardwareBuffer::HBL_DISCARD));
      for(int ypos = 0; ypos < GRID_PATCH_VERTICES; ++ypos) {  
        for(int xpos = 0; xpos < GRID_PATCH_VERTICES; ++xpos) {
          Real positionX = radius * cos(latitude.valueRadians()) * sin(longitude.valueRadians());
          Real positionY = radius * sin(latitude.valueRadians());
          Real positionZ = radius * cos(latitude.valueRadians()) * cos(longitude.valueRadians());
          
          Vector normal(positionX,positionY, positionZ);
          normal.normalise();
          
          Real textureU = Real(xpos) / GRID_SUBDIVISION;
          Real textureV = Real(ypos) / GRID_SUBDIVISION;
          
          *vertexData++ = positionX;
          *vertexData++ = positionY;
          *vertexData++ = positionZ;
          
          *vertexData++ = normal[0];
          *vertexData++ = normal[1];
          *vertexData++ = normal[2];
          
          // UV 1
          *vertexData++ = textureU;
          *vertexData++ = textureV;

          // UV 2
          *vertexData++ = 0;
          *vertexData++ = 0;
          *vertexData++ = 0;

          subMeshBounds.merge(Vector(positionX, positionY, positionZ));
          
          longitude += longitudeStep;
        }
        longitude = startLongitude;
        latitude += latitudeStep;
      }
      hwBuffer->unlock();

      Sphere& subMeshSphere = getSubMeshBounds(subx, suby);
      const Vector center = subMeshBounds.getCenter();
      Vector diagonal = subMeshBounds.getMaximum() - center;
      subMeshSphere.setCenter(center);
      subMeshSphere.setRadius(diagonal.length());

      meshBounds.merge(subMeshBounds);
    }
  }
  
  mesh->_setBounds(meshBounds);
  mesh->buildTangentVectors(0, 1);
}


//--------------------------------------------------------------------------------


bool
PlanetMesh::isWithinEffectiveBounds()
{
  Degree angle = getAngle(observerPosition);
  return (angle.valueDegrees() < effectiveAngle.valueDegrees());
}

bool
PlanetMesh::movingAway()
{
  Vector deltaPosition = Quaternion(Radian(0.1), orbitalAxis) * observerPosition;
  Vector moveDirection = deltaPosition - observerPosition;
  Vector meshDirection = meshPosition - observerPosition;

  moveDirection.normalise();
  meshDirection.normalise();
  
  return moveDirection.dotProduct(meshDirection) < 0.0f;
}


Degree
PlanetMesh::getAngle(const Vector& observerPosition)
{
  Radian angle(acos(observerPosition.normalisedCopy().dotProduct(meshPosition.normalisedCopy())));
  return Degree(angle.valueDegrees());
}

//--------------------------------------------------------------------------------

Degree
PlanetMesh::getLongitude()
{
  return getLongitude(observerPosition);
}

Degree
PlanetMesh::getLatitude()
{
  return getLatitude(observerPosition);
}



Degree
PlanetMesh::getLongitude(Vector& position)
{
  Vector normalizedPosition = position;
  normalizedPosition.normalise();

  Vector equatorialPlanePosition(position[0], 0, position[2]);
  equatorialPlanePosition.normalise();
  Degree longitude;

  // Test for singularity at poles
  if(equatorialPlanePosition.length() != 0) {
    longitude = Radian(acos(Vector::UNIT_Z.dotProduct(equatorialPlanePosition)));
    if(position[0] < 0) longitude = -longitude;
  }
  else {
    longitude = 0;
  }
  return longitude;
}

Degree
PlanetMesh::getLatitude(Vector& position)
{
  Vector normalizedPosition = position;
  normalizedPosition.normalise();
  Degree latitude = Radian(asin(normalizedPosition[1]));

  return latitude;
}


//--------------------------------------------------------------------------------
// Output

ostream& operator<<(ostream& out, PlanetMesh* planetMesh)
{
  out << "MeshPosition: " << endl << planetMesh->meshPosition << endl
      << "Distance: " << (planetMesh->meshPosition - planetMesh->observerPosition).length() <<  endl
      << "WithinBounds: " << (planetMesh->isWithinEffectiveBounds() ? "TRUE" : "FALSE") << endl
      << "MovingAway: " << (planetMesh->movingAway() ? "TRUE" : "FALSE") << endl
      << "EffectiveAngle: " << planetMesh->effectiveAngle.valueDegrees() << endl
      << "CurrentAngle: " << planetMesh->getAngle(planetMesh->observerPosition).valueDegrees() << endl
      << "[TexturePatch]" << endl << planetMesh->diffuseTexturePatch << endl;

  return out;
}

