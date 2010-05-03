//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Port.hpp"
#include "Module.hpp"

//--------------------------------------------------------------------------------
// Constructors


Port::Port(const Vector& position, const Quaternion& orientation)
{
  setPosition(position);
  setOrientation(orientation);
}

//--------------------------------------------------------------------------------

// const string&
// Port::getName() const
// {
//   return name;
// }


// Module*
// Port::getHost() const
// {
//   return mModule;
// }


//--------------------------------------------------------------------------------

// void
// Port::attachPort(SceneNode* hostNode, string moduleEntityName)
// {
//   if(sceneNode != NULL)
//     CHECK_FALSE("Attaching port that already has SceneNode");

//   SceneManager* creator = hostNode->getCreator();
  
//   // The port handle
//   {    
//     string entityName = string(moduleEntityName + "-port-" + name);
//     portHandle = creator->createEntity(entityName, "user_interface/port.mesh");
//     portHandle->setUserObject(this);
//     portHandle->setVisible(false);
//     connectionMarker = creator->createEntity(entityName + "-marker", "user_interface/mesh/ConnectionMarker.mesh");
//     connectionMarker->setVisible(false);
    
//     sceneNode = hostNode->createChildSceneNode();
//     sceneNode->attachObject(portHandle);
//     sceneNode->attachObject(connectionMarker);
//     sceneNode->setPosition(position);
//     sceneNode->setOrientation(orientation);
//   }
  
//   // The flow marker
//   {
//     flowMarker = creator->createEntity(string(moduleEntityName + "-port-" + name + "-connection"), "user_interface/mesh/PortMarker.mesh");
//     flowMarker->setVisible(false);

//     flowNode = hostNode->createChildSceneNode();
//     flowNode->attachObject(flowMarker);
//     flowNode->setPosition(Vector::ZERO);
//     flowNode->setOrientation(Quaternion::IDENTITY);
    
//     // Orient the marker to point towards the port
//     Vector portDirection = position;
//     portDirection.normalise();
//     Quaternion rotQuaternion;
    
//     if ((Vector::UNIT_Z + portDirection).squaredLength() <  0.00005f) {
//       rotQuaternion.FromAngleAxis(Radian(Math::PI), Vector::UNIT_Y);
//     }
//     else {
//       rotQuaternion = Vector::UNIT_Z.getRotationTo(portDirection);
//     }
//     flowNode->setOrientation(rotQuaternion);
//     flowNode->setScale(1, 1, position.length());
//   }
// }


// void
// Port::detachPort()
// {
//   if(sceneNode != NULL) {    
//     SceneManager* sceneManager = sceneNode->getCreator();
//     sceneNode->getParentSceneNode()->removeChild(sceneNode);
//     delete sceneNode;
    
//     sceneManager->removeEntity(portHandle);
//   }
//   if(flowNode != NULL) {
//     SceneManager* sceneManager = flowNode->getCreator();
//     flowNode->getParentSceneNode()->removeChild(flowNode);
//     delete flowNode;
//     sceneManager->removeEntity(flowMarker);
//   } 
  
//   sceneNode = NULL;
//   portHandle = NULL;
//   flowNode = NULL;
//   flowMarker = NULL;
// }

//--------------------------------------------------------------------------------

// void
// Port::setHandleVisible(bool visible)
// {
//   if(portHandle != NULL) {
//     if(!isConnected())
//       portHandle->setVisible(visible);
//     else
//       portHandle->setVisible(false);
//   }
// }

// void
// Port::setFlowMarkerVisible(bool visible)
// {
//   if(flowMarker != NULL) {
//     if(isConnected())
//       flowMarker->setVisible(visible);
//     else
//       flowMarker->setVisible(false);
//   }
// }


// void
// Port::setConnectionMarkerVisible(bool visible)
// {
//   if(connectionMarker != NULL) {
//     connectionMarker->setVisible(visible);
//   }
// }

//--------------------------------------------------------------------------------

// bool
// Port::isConnected()
// {
//   return mConnection != NULL;
// }


// void
// Port::connect(Port* newConnection)
// {
//   CHECK("Valid other Port", newConnection != NULL);
//   if(isConnected() || newConnection->isConnected()) return;
// //   CHECK("Other port has same Datafile", getDataFile() == newConnection->getDataFile());

//   if(false == canConnect(newConnection)) return;
  
//   newConnection->mConnection = this;
//   mConnection = newConnection;
// }


// void
// Port::disconnect()
// {
//   if(mConnection != NULL && mConnection->mConnection == this) {
//     mConnection->mConnection = NULL;    
//   }
//   mConnection = NULL;
// }

//--------------------------------------------------------------------------------
// Checks if we can connect to the other port

// REFACTOR: THese tests should use prototypes

// bool
// Port::canConnect(Port* )
// {
//   return canConnect(otherPort, mModule->getPosition(), mModule->getOrientation(), otherPort->mModule->getPosition(), otherPort->mModule->getOrientation());
//   return false;
// }

// REFACTOR: this is probably the real test.

bool
Port::canConnect(Port* otherPort, const Matrix& myTransform, const Matrix& otherTransform)
{
  const Real MAGIC_PORT_ACCEPTANCE_DISTANCE = 0.1;

  //Only check positions for now ..
  Vector myPosition = position * myTransform;
  Vector otherPosition = otherPort->position * otherTransform;
  Vector displacement = myPosition - otherPosition;
  return displacement.length() < MAGIC_PORT_ACCEPTANCE_DISTANCE;
}


// bool
// Port::canConnect(Port* otherPort, const Vector& module1Position, const Quaternion& module1Orientation,
//                  const Vector& module2Position, const Quaternion& module2Orientation) const
// {
//   const Real MAGIC_PORT_ACCEPTANCE_DISTANCE = 0.1;

//   //Only check positions for now ..
//   Vector myPosition = (position * module1Orientation) + module1Position;
//   Vector otherPosition = (otherPort->position * module2Orientation) + module2Position;
//   Vector displacement = myPosition - otherPosition;
//   return displacement.length() < MAGIC_PORT_ACCEPTANCE_DISTANCE;
// }


//--------------------------------------------------------------------------------

const Vector&
Port::getPosition() const
{
  return position;
}

const Quaternion&
Port::getOrientation() const
{
  return orientation;
}


void
Port::setPosition(const Vector& newPosition)
{
  position = newPosition;
}


void
Port::setOrientation(const Quaternion& newOrientation)
{
  orientation = newOrientation;
}

//--------------------------------------------------------------------------------

Matrix
Port::getTransform() const
{
  Matrix transform(getPosition(), getOrientation());
  return transform;
}


void
Port::setTransform(const Matrix& transform)
{
  setOrientation(transform.getOrientation());
  setPosition(transform.getPosition());
}


