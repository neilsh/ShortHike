//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "CollisionManager.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> CollisionManager*
Singleton<CollisionManager>::ms_Singleton = 0;


//--------------------------------------------------------------------------------

CollisionManager::CollisionManager()
{
}

//--------------------------------------------------------------------------------

void
CollisionManager::addMesh(string meshName, string groupName)
{
  MeshPtr mesh = MeshManager::getSingleton().load(meshName, groupName);
  addMesh(mesh);
}


//--------------------------------------------------------------------------------

#pragma warning(disable: 4018 4189)

void
CollisionManager::addMesh(MeshPtr pMesh)
{
  using ColDet::CollisionModel3D;
  
  if(modelMap.find(pMesh->getName()) != modelMap.end()) return;
  
  unsigned int numVertices = 0; 
  unsigned int numIndices = 0;  

  // create a new collision model 
  CollisionModel3D* mCollisionModel = ColDet::newCollisionModel3D(); 

  unsigned int i = 0, j = 0; 
  for(i = 0; i < pMesh->getNumSubMeshes(); i++) 
  { 
    SubMesh* pSubMesh = pMesh->getSubMesh(i);        
    if(pSubMesh->vertexData != NULL)
      numVertices += pSubMesh->vertexData->vertexCount;    
    if(pSubMesh->indexData != NULL)
      numIndices += pSubMesh->indexData->indexCount; 
  }  

  // set the number of triangles 
  mCollisionModel->setTriangleNumber(numIndices / 3); 

  // Count the number of vertices and incides so we can Set them 
  unsigned int indexCount = 0, vertListCount = 0;  

  for(i = 0; i < pMesh->getNumSubMeshes(); i++) 
  { 
    SubMesh* pSubMesh = pMesh->getSubMesh(i);    

    if(pSubMesh->vertexData != NULL) {
      const VertexElement* elem = pSubMesh->vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION); 
      HardwareVertexBufferSharedPtr vbuf = pSubMesh->vertexData->vertexBufferBinding->getBuffer(elem->getSource()); 
      
      Real* test = new Real[pSubMesh->vertexData->vertexCount * 3]; 
      float* pDest = test; 
      
      unsigned char* pData = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY)); 
      
      for (j = 0; j < pSubMesh->vertexData->vertexCount; j++) { 
        float* pFloat; 
        elem->baseVertexPointerToElement(pData, &pFloat); 
        
        *pDest++ = *pFloat++; 
        *pDest++ = *pFloat++; 
        *pDest++ = *pFloat++; 
        
        pData += vbuf->getVertexSize(); 
      } 
      
      vbuf->unlock(); 

      if(pSubMesh->indexData != NULL) {
        HardwareIndexBufferSharedPtr ibuf = pSubMesh->indexData->indexBuffer; 
        unsigned short* test2 = new unsigned short[pSubMesh->indexData->indexCount];  
        
        ibuf->readData(0, ibuf->getSizeInBytes(), test2); 
        
        float* tri[3];
        
        for (int j = 0; j < pSubMesh->indexData->indexCount; j = j + 3) {      
          for (int k = 0; k < 3; k++) {          
            tri[k] = new float[3]; 
            tri[k][0] =  test[(test2[j + k] * 3) + 0]; 
            tri[k][1] =  test[(test2[j + k] * 3) + 1]; 
            tri[k][2] =  test[(test2[j + k] * 3) + 2]; 
          } 
          mCollisionModel->addTriangle(tri[0], tri[1], tri[2]);      
          
          delete[] tri[0]; 
          delete[] tri[1]; 
          delete[] tri[2]; 
        } 
        delete[] test2;    
      } 
      delete[] test; 
    }
  }
    
  mCollisionModel->finalize();  

  modelMap[pMesh->getName()] = mCollisionModel;
}

//--------------------------------------------------------------------------------

const AxisAlignedBox&
CollisionManager::getBoundingBox(string meshName, string groupName)
{
  MeshPtr mesh = MeshManager::getSingleton().load(meshName, groupName);
  return mesh->getBounds();
}


Real
CollisionManager::getBoundingRadius(string meshName, string groupName)
{
  MeshPtr mesh = MeshManager::getSingleton().load(meshName, groupName);
  return mesh->getBoundingSphereRadius();
}

//--------------------------------------------------------------------------------

bool
CollisionManager::isIntersecting(const Ray& ray, Entity* entity, Real& distance)
{
  // get a pointer to the collision model 
  ColDet::CollisionModel3D* mColModel = modelMap[entity->getMesh()->getName()];
  if(mColModel == NULL) return false;
      
  // set the world transform for the entity    
  {
    Matrix4 world; 
    entity->getParentSceneNode()->getWorldTransforms(&world);  
    
    float fMatrix[16];
    fMatrix[0] = world[0][0];
    fMatrix[1] = world[1][0];
    fMatrix[2] = world[2][0];
    fMatrix[3] = world[3][0];
    fMatrix[4] = world[0][1];
    fMatrix[5] = world[1][1];
    fMatrix[6] = world[2][1];
    fMatrix[7] = world[3][1];
    fMatrix[8] = world[0][2];
    fMatrix[9] = world[1][2];
    fMatrix[10] = world[2][2];
    fMatrix[11] = world[3][2];
    fMatrix[12] = world[0][3];
    fMatrix[13] = world[1][3];
    fMatrix[14] = world[2][3];
    fMatrix[15] = world[3][3];

    mColModel->setTransform(fMatrix); 
  }
  
  // convert the ray  
  float Origin[3], Direction[3]; 
  
  Origin[0] = ray.getOrigin().x; 
  Origin[1] = ray.getOrigin().y; 
  Origin[2] = ray.getOrigin().z; 

  Direction[0] = ray.getDirection().x; 
  Direction[1] = ray.getDirection().y; 
  Direction[2] = ray.getDirection().z;  

  // check for a collision 
  bool col = mColModel->rayCollision(Origin, Direction);      

  // for testing purposes 
//   mColModel->getCollidingTriangles(t1, t2, false); 
//   mColModel->getCollisionPoint(colPoint, false); 
  float collisionPoint[3];
  mColModel->getCollisionPoint(collisionPoint, false);
  Vector displacement = ray.getOrigin() - Vector(collisionPoint[0], collisionPoint[1], collisionPoint[2]);
  distance = displacement.length();

  return col; 
}
