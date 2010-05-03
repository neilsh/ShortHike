//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "OrbitSelectedState.hpp"
#include "Orbit.hpp"
#include "audio/SoundManager.hpp"
#include "config/ConfigManager.hpp"
#include "config/Player.hpp"
#include "game/Station.hpp"
#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "game/Port.hpp"
#include "guilib/GUIManager.hpp"
#include "main/Main.hpp"

//--------------------------------------------------------------------------------

OrbitSelectedState::OrbitSelectedState(Orbit* _orbitScreen)
  : OrbitState(_orbitScreen)
{
}

//--------------------------------------------------------------------------------

void
OrbitSelectedState::enterState()
{
}

void
OrbitSelectedState::exitState()
{
  clearSelection();
}


//--------------------------------------------------------------------------------

void
OrbitSelectedState::setSelection(Module* target)
{
  clearSelection();
  if(target == NULL) return;
  addSelection(target);
}

void
OrbitSelectedState::addSelection(Module* target)
{
  selection.insert(target);
  target->setSelectionVisible(true);
  updateHandles();
}


void
OrbitSelectedState::removeSelection(Module* target)
{
  selection.erase(target);
  target->setSelectionVisible(false);
  updateHandles();
}


void
OrbitSelectedState::clearSelection()
{
  for(set<Module*>::const_iterator iModule = selection.begin(); iModule != selection.end(); ++iModule) {
    Module* currentModule = *iModule;
    currentModule->setSelectionVisible(false);
  }
  selection.clear();
}


void
OrbitSelectedState::emptySelection()
{
  selection.clear();
}


Module*
OrbitSelectedState::getFirstModule()
{
  if(0 == selection.size()) return NULL;
  else return *(selection.begin());
}


//--------------------------------------------------------------------------------

bool
OrbitSelectedState::keyboardEvent(char keyCode, bool pressed)
{
  if(pressed && keyCode == VK_DELETE) {
    // Can't delete all modules
    if(orbitScreen->station->getModuleCount() == selection.size()) return true;

    Module* nextSelection = NULL;
    for(set<Module*>::const_iterator iModule = selection.begin(); iModule != selection.end(); ++iModule) {
      Module* currentModule = *iModule;

      // Find the potential next module to select
      if(nextSelection == NULL) {
        PortList ports = currentModule->getPorts();
        for(PortIterator iPort = ports.begin(); iPort != ports.end(); ++iPort) {
          Port* currentPort = *iPort;
          if(currentPort->isConnected()) {
            Module* target = currentPort->getConnection()->getHost();
            if(selection.find(target) == selection.end()) {
              nextSelection = target;
              break;
            }
          }
        }
      }      
       
      rMain()->getMission()->useActionPoint();
      // Profile: Update number of modules removed
      ConfigManager::getPlayer()->mModulesDeleted++;

      orbitScreen->station->removeModule(currentModule);
    }

    emptySelection();      
    if(nextSelection == NULL) {
      orbitScreen->stateManager.setCurrentState(Orbit::READY_STATE);
    }
    else {
      setSelection(nextSelection);
    }
    
    SoundManager::playSoundEffect("orbit/undocking");
    return true;
  }  

  //--------------------------------------------------------------------------------
  // Module assembly
  if(pressed && keyCode == VK_SPACE && 1 < selection.size()) {
    set<Module*> parts = selection;
    Module* assembly = orbitScreen->station->assembleModules(parts);
    if(assembly != NULL) {
      emptySelection();
      setSelection(assembly);

      // Profile: Update number of assemblies created
      ConfigManager::getPlayer()->mAssembliesCreated++;
    }
    return true;
  }

  



  if(1 == selection.size()) {
    Module* selectedModule = getFirstModule();

    //--------------------------------------------------------------------------------
    // Storage of quick prototypes

    if(pressed && (rGUIManager()->getKeyModifier() & KEY_MODIFIER_SHIFT)
       && (keyCode == '1' || keyCode == '2' || keyCode == '3' || keyCode == '4' || keyCode == '5')) {
      int quickIndex = keyCode - '1';
      if(orbitScreen->quickPrototypes[quickIndex] != NULL) Item::destroyItem(orbitScreen->quickPrototypes[quickIndex]);
      Module* prototype = static_cast<Module*>(selectedModule->createInstance(NULL));
      prototype->disconnectPorts();
      orbitScreen->quickPrototypes[quickIndex] = prototype;
      SoundManager::playSoundEffect("guilib/button_click");  
      return true;
    }
    
    //--------------------------------------------------------------------------------
    // Rotation
    
    float angle = 45;
    if(rGUIManager()->getKeyModifier() & KEY_MODIFIER_SHIFT)
      angle = angle / 4;

    if(pressed && 'X' == keyCode) {
      selectedModule->rotate(Degree(-angle));

      // Profile: Update revolutions rotated
      ConfigManager::getPlayer()->mRevolutionsRotated += angle / 360.0;
      return true;
    }
    else if(pressed && 'C' == keyCode) {
      selectedModule->rotate(Degree(angle));

      // Profile: Update revolutions rotated
      ConfigManager::getPlayer()->mRevolutionsRotated += angle / 360.0;
      return true;
    }
  }


  
  return OrbitState::keyboardEvent(keyCode, pressed);
}


//--------------------------------------------------------------------------------

bool
OrbitSelectedState::mouseClickEvent(int buttonID, unsigned clickCount, int x, int y)
{
  if(buttonID == 0) {
    if(rGUIManager()->getKeyModifier() & KEY_MODIFIER_SHIFT) {
      Item* target = pickItem(x, y);
      if(target != NULL) {
        Module* targetModule = target->castToClass<Module>();
        if(targetModule != NULL) {
          if(find(selection.begin(), selection.end(), targetModule) == selection.end()) {
            addSelection(targetModule);
            return true;
          }
          else {
            removeSelection(targetModule);
            if(0 == selection.size())
              orbitScreen->stateManager.setCurrentState(Orbit::READY_STATE);
            return true;
          }
        }
      }
    }
    else if(1 == selection.size()) {
      Item* target = pickItem(x, y);
      if(target != NULL) {
        Port* targetPort = target->castToClass<Port>();
        if(targetPort != NULL && targetPort->isConnected() == false && targetPort->getHost() != getFirstModule()) {
          Module* selectedModule = getFirstModule();
          Station* station = orbitScreen->getStation();
          station->disconnectModule(selectedModule);
          station->connectModule(selectedModule, selectedModule->getDefaultPort(), targetPort);
          updateHandles();
          SoundManager::playSoundEffect("orbit/docking");

          rMain()->getMission()->useActionPoint();
          // Profile: Update number of modules moved
          ConfigManager::getPlayer()->mModulesMoved++;

          return true;
        }
      }
    }
  }
  
  return OrbitState::mouseClickEvent(buttonID, clickCount, x, y);  
}


//--------------------------------------------------------------------------------

void
OrbitSelectedState::updateHandles()
{
  if(1 == selection.size()) {
    Module* selectedModule = getFirstModule();
    orbitScreen->getStation()->setPortHandlesVisible(true);

    PortList ports = selectedModule->getPorts();
    for(PortList::const_iterator portI = ports.begin(); portI != ports.end(); ++portI) {
      Port* currentPort = *portI;
      currentPort->setHandleVisible(false);
    }
  }
  else {
    orbitScreen->getStation()->setPortHandlesVisible(false);
  }
}


//--------------------------------------------------------------------------------
// Output

void
OrbitSelectedState::outputString(wostream& out)
{
  Module* selectedModule = getFirstModule();
  CHECK("SelectedModule is not null", selectedModule != NULL);
  out << stringToWString(selectedModule->getName()) << " ("
      << setprecision(1) << fixed << selectedModule->getMass() << " ton)" << endl;
  if(selectedModule->isOnline()) {
    out << "[Online]" << endl;
  }
  else {
    out << "[OFFLINE]" << endl;
  }  
  out << "Maintenance: $" << setprecision(2) << selectedModule->getMaintenanceExpenses() << "M/year" << endl;

  out << "LifeSupport: " << setprecision(2) << selectedModule->getLifeSupport() << L"/"
      << setprecision(2) << selectedModule->getLifeSupportBase() << endl;
  out << "Stress: " << setprecision(2) << selectedModule->getStress() << L"/"
      << setprecision(2) << selectedModule->getStressBase() << endl;
  // FIXME:
  out << "Flow: " << setprecision(1) << selectedModule->getFlowTotal() << endl;

  for(int iResource = DISCRETE_FLOW_INITIAL; iResource < DISCRETE_NUM; ++iResource) {
    DiscreteResource resource = static_cast<DiscreteResource>(iResource);
    out << toString(resource).c_str() << L" " << fixed << setprecision(2) << selectedModule->getDiscreteCurrent(resource)
        << " " << setprecision(2) << selectedModule->getDiscreteTarget(resource)
        << " (" << setprecision(1) << selectedModule->getFlowLocal(resource) << ")"
        << endl;
  }
  

//   out << "- Production: " << endl;
//   for(FlowResource type = FLOW_INITIAL; type < FLOW_NUM; ++type) {
//     if(selectedModule->getOutputCapacity(type) > 0) {
//       Real totalFlow = selectedModule->getOutputCurrent(type);
//       Real totalCapacity = selectedModule->getOutputCapacity(type);
//       Real utilization = (totalFlow / totalCapacity * 100.0f);
//       out << stringToWString(flowResourceToString(type)) << ": " << fixed
//           << setprecision(1) << totalFlow << "/" << totalCapacity << " " << flowResourceUnits(type)
//           << " "
//           << setprecision(0) << utilization << "%"
//           << endl;
//     }
//   }
//   out << "- Consumption: " << endl;
//   for(FlowResource type = FLOW_INITIAL; type < FLOW_NUM; ++type) {
//     if(selectedModule->getInputCapacity(type) > 0) {
//       Real totalFlow = selectedModule->getInputCurrent(type);
//       Real totalCapacity = selectedModule->getInputCapacity(type);
//       Real utilization = (totalFlow / totalCapacity * 100.0f);
//       out << stringToWString(flowResourceToString(type)) << ": " << fixed
//           << setprecision(1) << totalFlow << "/" << totalCapacity << " " << flowResourceUnits(type)
//           << " "
//           << setprecision(0) << utilization << "%"
//           << endl;
//     }
//   }  
}
