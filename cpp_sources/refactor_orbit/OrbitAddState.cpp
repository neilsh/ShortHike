//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "OrbitAddState.hpp"
#include "Orbit.hpp"
#include "audio/SoundManager.hpp"
#include "config/ConfigManager.hpp"
#include "config/Player.hpp"
#include "game/Aspects.hpp"
#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "game/Port.hpp"
#include "game/Station.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/Point.hpp"
#include "main/Main.hpp"

//--------------------------------------------------------------------------------

OrbitAddState::OrbitAddState(Orbit* _orbitScreen)
  : OrbitState(_orbitScreen), currentPrototype(NULL), prototypeStack0(NULL), prototypeStack1(NULL)
{
  addEntity = orbitScreen->getSceneManager()->createEntity("OrbitSelectedState::addEntity", "user_interface/selection.mesh");
  addEntity->setMaterialName("UserInterface_ModuleNeutral");
  addNodeModule = orbitScreen->rootNode->createChildSceneNode();
  addNodeBox = addNodeModule->createChildSceneNode();
  addNodeBox->attachObject(addEntity);
  addNodeBox->setScale(20, 20, 20);
  addNodeModule->setVisible(false);
}

//--------------------------------------------------------------------------------

void
OrbitAddState::enterState()
{
  currentPrototype = NULL;
  orbitScreen->getStation()->setPortHandlesVisible(true);
  setTargetPort(NULL);
  mTargetPortDirty = true;
}

void
OrbitAddState::exitState()
{
  currentPrototype = NULL;
  setTargetPort(NULL);
  orbitScreen->getStation()->setPortHandlesVisible(false);
}

//--------------------------------------------------------------------------------

void
OrbitAddState::setPrototype(Module* _prototype)
{
  currentPrototype = _prototype;
  connectionPort = currentPrototype->getDefaultPort();

  const AxisAlignedBox& bounds = currentPrototype->getBoundingBox();
  const Vector& min = bounds.getMinimum();
  const Vector& max = bounds.getMaximum();
  addNodeBox->setScale(abs(max[0] - min[0]), abs(max[1] - min[1]), abs(max[2] - min[2]));
  addNodeBox->setPosition((max[0] + min[0]) * 0.5, (max[1] + min[1]) * 0.5, (max[2] + min[2]) * 0.5);

  // New stuff to communicate the proper module position
  Matrix scaleMatrix = Matrix::IDENTITY;
  scaleMatrix.setScale(Vector(abs(max[0] - min[0]), abs(max[1] - min[1]), abs(max[2] - min[2])));

  Matrix positionMatrix;
  positionMatrix.makeTrans((max[0] + min[0]) * 0.5, (max[1] + min[1]) * 0.5, (max[2] + min[2]) * 0.5);
  mModuleCenterTransform = positionMatrix * scaleMatrix;

  if(prototypeStack0 != currentPrototype) {
    prototypeStack1 = prototypeStack0;
    prototypeStack0 = currentPrototype;
  }
  orientMarker();
}

const Module*
OrbitAddState::getLastPrototype()
{
  if(currentPrototype == NULL) {
    if(NULL != prototypeStack0) {
      return prototypeStack0;
    }
  }
  else {
    if(NULL != prototypeStack1) {
      return prototypeStack1;
    }
  }
  return NULL;
}


//--------------------------------------------------------------------------------

bool
OrbitAddState::mouseClickEvent(int buttonID, unsigned clickCount, int x, int y)
{

  if(buttonID == 0 && currentPrototype != NULL) {
    Item* target = pickItem(x, y);
    if(target != NULL) {
      Port* targetPort = target->castToClass<Port>();
      if(targetPort != NULL) {

        if(rMain()->getMission()->isSolvent() == true && targetPort != NULL && targetPort->isConnected() == false) {
          SoundManager::playSoundEffect("orbit/docking");
          Module * target =  static_cast<Module*>(currentPrototype->createInstance(orbitScreen->station->getDataFile()));
          orbitScreen->station->addModule(target, connectionPort, targetPort);
          if(rMain()->getAspects()->isCredits()) {
            rMain()->getMission()->creditTransaction(-target->getHardwareCost());
          }
          rMain()->getMission()->useActionPoint();
          target->rotate(rotateDegree);
          
          // Show the ports on this module
          PortList ports = target->getPorts();
          for(PortList::const_iterator portI = ports.begin(); portI != ports.end(); ++portI) {
            Port* currentPort = *portI;
            currentPort->setHandleVisible(true);
          }

          setTargetPort(NULL);
          updateTargetPort(x, y);
          
          // --------------------------------------------------------------------------------
          // Profile: Update number of modules added
          ConfigManager::getPlayer()->mModulesAdded++;
          return true;
        }
      }
    }    
  }  
  return OrbitState::mouseClickEvent(buttonID, clickCount, x, y);
}

//--------------------------------------------------------------------------------


void
OrbitAddState::mouseMotionEvent(int x, int y, float deltaX, float deltaY , float deltaWheel)
{
  OrbitState::mouseMotionEvent(x, y, deltaX, deltaY, deltaWheel);
  mTargetPortDirty = true;
}


void
OrbitAddState::update(float)
{
  if(mTargetPortDirty) {
    Point cursorPosition = rGUIManager()->getCursorPosition();
    updateTargetPort(cursorPosition.x, cursorPosition.y);
    mTargetPortDirty = false;
  }  
}


//--------------------------------------------------------------------------------

bool
OrbitAddState::keyboardEvent(char keyCode, bool pressed)
{
  if(pressed && 'X' == keyCode) {
    rotateDegree += Degree(-45);
    orientMarker();
    return true;
  }
  else if(pressed && 'C' == keyCode) {
    rotateDegree += Degree(45);
    orientMarker();
    return true;
  }
  else if(pressed && 'V' == keyCode) {
    selectNextConnectionPort();
    orientMarker();
    return true;
  }
  
  return OrbitState::keyboardEvent(keyCode, pressed);
}

//--------------------------------------------------------------------------------

void
OrbitAddState::updateTargetPort(int x, int y)
{
  Item* target = pickItem(x, y);
  if(target != NULL) {
    Port* port = target->castToClass<Port>();
    if(port != NULL) {
      setTargetPort(port);
      return;
    }
  }
  setTargetPort(NULL);
}


//--------------------------------------------------------------------------------

void
OrbitAddState::setTargetPort(Port* _newTarget)
{
  if(targetPort == _newTarget) return;
  rotateDegree = Degree(0);
  
  if(targetPort != NULL) {
    for(set<pair<Port*, Port*> >::const_iterator portPairI = connections.begin(); portPairI != connections.end(); ++portPairI) {
      Port* otherPort = (*portPairI).second;
      otherPort->setConnectionMarkerVisible(false);
    }
    connections.clear();
  }
  targetPort = _newTarget;
  
  orientMarker();
  if(targetPort != NULL) {
    Quaternion moduleOrientation;
    Vector modulePosition;
    currentPrototype->findTransformAgainstPort(connectionPort, targetPort, modulePosition, moduleOrientation);
    
    connections = currentPrototype->findConnectingPorts(orbitScreen->getStation()->getModules(), modulePosition, moduleOrientation);
    for(set<pair<Port*, Port*> >::const_iterator portPairI = connections.begin(); portPairI != connections.end(); ++portPairI) {
      Port* otherPort = (*portPairI).second;
      otherPort->setConnectionMarkerVisible(true);
    }    

    SoundManager::playSoundEffect("guilib/button_rollover");  
  }
}

//--------------------------------------------------------------------------------

void
OrbitAddState::orientMarker()
{
  while(rotateDegree < Degree(0)) rotateDegree += Degree(360);
  while(rotateDegree > Degree(360)) rotateDegree -= Degree(360);
  if(targetPort != NULL) {
    Quaternion moduleOrientation;
    Vector modulePosition;
    currentPrototype->findTransformAgainstPort(connectionPort, targetPort, modulePosition, moduleOrientation, rotateDegree);
    addNodeModule->setPosition(modulePosition);
    addNodeModule->setOrientation(moduleOrientation);
    addNodeModule->setVisible(true);
    
    Matrix moduleTranslate;
    moduleTranslate.makeTrans(modulePosition);
    Matrix moduleRotate(moduleOrientation);
    mPositionMarkerTransform = moduleTranslate * moduleRotate * mModuleCenterTransform;
    mPositionMarkerVisible = true;
  }
  else {
    addNodeModule->setVisible(false);
    mPositionMarkerVisible = false;
  }
}


//--------------------------------------------------------------------------------

void
OrbitAddState::selectNextConnectionPort()
{
  if(currentPrototype != NULL) {
    PortList ports = currentPrototype->getPorts();
    PortIterator iCurrentPort = find(ports.begin(), ports.end(), connectionPort);
    iCurrentPort++;
    if(iCurrentPort == ports.end()) iCurrentPort = ports.begin();
    connectionPort = *iCurrentPort;   
  }
}


//--------------------------------------------------------------------------------
// Output

void
OrbitAddState::outputString(wostream& out)
{
  if(currentPrototype != NULL) {
    Real mass = currentPrototype->getMass();
    out << stringToWString(currentPrototype->getName()) << " ("
        << setprecision(1) << fixed << mass << " ton)" << endl;
    out << "Cost: $" << currentPrototype->getHardwareCost() << "M ($"
        << setprecision(2) << currentPrototype->getMaintenanceExpenses() << "M/year)" << endl;
    
    out << "- Production: " << setprecision(1) << endl;
    // FIXME:
//     for(FlowResource type = FLOW_INITIAL; type < FLOW_NUM; ++type) {
//       if(simUnit->getOutputCapacity(type) > 0) {
//         Real totalCapacity = simUnit->getOutputCapacity(type);
//         out << stringToWString(flowResourceToString(type)) << ": " << fixed
//             << setprecision(1) << totalCapacity << " " << flowResourceUnits(type) << endl;
//       }
//     }  
    out << "- Consumption: " << endl;
    // FIXME:
//     for(FlowResource type = FLOW_INITIAL; type < FLOW_NUM; ++type) {
//       if(simUnit->getInputCapacity(type) > 0) {
//         Real totalCapacity = simUnit->getInputCapacity(type);
//         out << stringToWString(flowResourceToString(type)) << ": " << fixed
//             << setprecision(1) << totalCapacity << " " << flowResourceUnits(type) << endl;
//       }
//     }  
  }
}

