//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "LayerDock.hpp"
#include "OrbitMap.hpp"
#include "Orbit.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/Frame.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/Label.hpp"
#include "guilib/Button.hpp"
#include "game/Aspects.hpp"
#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "game/Port.hpp"
#include "game/Station.hpp"
#include "main/Main.hpp"
#include "widgets/ScriptButton.hpp"

//--------------------------------------------------------------------------------

LayerDock::LayerDock(Orbit* _orbitScreen, OrbitMap* orbitMap)
  : Frame("Orbit/LayerDock"), orbitScreen(_orbitScreen), layerName(L"Visual"), mShopButton(NULL)
{
  setBackground(new FrameBackground());

  layerInfo = new Label(L"Layer information");
  layerInfo->setVerticalAlignment(Label::TOP);

  visualButton = new Button(L"Visual", this);
  statusButton = new Button(L"Online", this);
  mMessagesButton = new ScriptButton(L"Messages", "message_reader_show()");
  mShopButton = new ScriptButton(L"Builders\nDilemma", "ui_show_shop()");

  GridLayout* layerLayout = new GridLayout(this, 32, 6);
  layerLayout->set_borders(2);
  layerLayout->set_padding(2);
  layerLayout->add(layerInfo, 0, 0, 22, 5);
  layerLayout->add(orbitMap, 22, 0, 10, 5);

  layerLayout->add(visualButton, 0, 5, 4, 1);
  layerLayout->add(statusButton, 4, 5, 4, 1);
  layerLayout->add(mMessagesButton, 8, 5, 4, 1);
  layerLayout->add(new ScriptButton(L"Missions", "game_mission_selector()"), 12, 5, 4, 1);
  layerLayout->add(new ScriptButton(L"Mission\nStatus", "ui_status_report_show()"), 16, 5, 4, 1);
  layerLayout->add(new ScriptButton(L"Profile", "ui_show_profile()"), 20, 5, 4, 1);
  layerLayout->add(mShopButton, 24, 5, 4, 1);
  layerLayout->add(new ScriptButton(L"Game", "ui_show_file_menu()"), 28, 5, 4, 1);
  
//   layerLayout->add(new Button(L"Energy"), 4, 5, 2, 1);
//   layerLayout->add(new Button(L"Thermal"), 6, 5, 2, 1);
//   layerLayout->add(new Button(L"ECLSS"), 8, 5, 2, 1);
//   layerLayout->add(new Button(L"Crew"), 10, 5, 2, 1);
}

//--------------------------------------------------------------------------------

void
LayerDock::update(Time)
{
  Station* station = orbitScreen->getStation();
  if(NULL == station) return;
  
  wostringstream layerInfoStream;
  layerInfoStream << layerName << endl;  

  unsigned int totalModules = station->getModuleCount();
  
  if(rMain()->getAspects()->isEnergy()) {
    float energyUtilization = fabs(station->getEnergyConsumption()) / station->getEnergyProduction() * 100;
    layerInfoStream << L"Energy: ";
    if(station->getEnergyOnlineCount() == totalModules)
      layerInfoStream << L"All online - ";
    else
      layerInfoStream << station->getEnergyOnlineCount() << L"/" << totalModules << L" online - ";
    if(station->getEnergyProduction() == 0)
      layerInfoStream << "No production" << endl;
    else
      layerInfoStream << fixed << setprecision(1) << fabs(station->getEnergyConsumption()) << L"/" << station->getEnergyProduction()
                      << L"kWe " << setprecision(0) << energyUtilization << L"%" << endl;
  }
  
  if(rMain()->getAspects()->isThermal()) {
    float thermalUtilization = fabs(station->getThermalConsumption()) / station->getThermalProduction() * 100;
    layerInfoStream << L"Thermal: ";
    if(station->getThermalOnlineCount() == totalModules)
      layerInfoStream << L"All online - ";
    else
      layerInfoStream << station->getThermalOnlineCount() << L"/" << totalModules << L" online - ";
    if(station->getThermalProduction() == 0)
      layerInfoStream << "No production" << endl;
    else
      layerInfoStream << fixed << setprecision(1) << fabs(station->getThermalConsumption()) << L"/" << station->getThermalProduction()
                      << L"kWt " << setprecision(0) << thermalUtilization << L"%" << endl;
  }
  
  layerInfo->setLabel(layerInfoStream.str());

  mShopButton->setVisible(rMain()->getAspects()->isShop());
}

//--------------------------------------------------------------------------------

void
LayerDock::actionPerformed(Widget* source)
{
  if(source == visualButton) {
    setMarkerVisible(false);
  }
  else if(source == statusButton) {
    setMarkerVisible(true);
  }  
}


//--------------------------------------------------------------------------------

void
LayerDock::setMarkerVisible(bool visible)
{
  const set<Module*>& modules = orbitScreen->getStation()->getModules();
  
  if(visible) {
    for(set<Module*>::const_iterator moduleI = modules.begin(); moduleI != modules.end(); ++moduleI) {
      Module* currentModule = *moduleI;
      currentModule->setHullVisible(false);
      PortList ports = currentModule->getPorts();
      for(PortList::const_iterator portI = ports.begin(); portI != ports.end(); ++portI) {
        Port* currentPort = *portI;
        currentPort->setFlowMarkerVisible(true);
      }
    }
  }
  else {
    for(set<Module*>::const_iterator moduleI = modules.begin(); moduleI != modules.end(); ++moduleI) {
      Module* currentModule = *moduleI;
      currentModule->setHullVisible(true);
      PortList ports = currentModule->getPorts();
      for(PortList::const_iterator portI = ports.begin(); portI != ports.end(); ++portI) {
        Port* currentPort = *portI;
        currentPort->setFlowMarkerVisible(false);
      }
    }
  }
}
