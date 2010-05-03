//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "InfoBrowser.hpp"
#include "BuildLayer.hpp"

#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "game/Prototype.hpp"
#include "game/Station.hpp"
#include "guilib/Button.hpp"
#include "guilib/Frame.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/Label.hpp"
#include "main/GamePlay.hpp"

// --------------------------------------------------------------------------------
// Singleton implementation

InfoBrowser*
rInfoBrowser()
{
  static InfoBrowser* mInfoBrowser = new InfoBrowser();
  return mInfoBrowser;
}

// --------------------------------------------------------------------------------

InfoBrowser::InfoBrowser()
  : mInfoFrame(NULL), mSelectionButton(NULL), mStationButton(NULL), mInfo(NULL)
{
  GridLayout* layout = new GridLayout(this, 32, 24);
  mInfoFrame = new Frame();
  mInfoFrame->setBackground(new FrameBackground());
  layout->add(mInfoFrame, 20, 0, 12, 20);
  mInfo = new Label(L"Info Browser");
  mInfoFrame->add(mInfo);
  setVisible(false);
}

InfoBrowser::~InfoBrowser()
{
  delete mInfoFrame;
  delete mSelectionButton;
  delete mStationButton;
  delete mInfo;
}


//--------------------------------------------------------------------------------

void
InfoBrowser::update(float)
{
  Mission* mission = rGamePlay()->getMission();
  Station* station = mission->getStation();
  wostringstream wossInfo;
  int moduleCount = station->getModuleCount();
  float income = station->getTotalIncome();
  float expenses = station->getTotalExpenses();
  float profit = income - expenses;
  wossInfo << "Station summary" << endl << endl;
  wossInfo << " Profit: $" << fixed << setprecision(1) << profit << "M Income: $" << income << "M Expenses: $" << expenses << "M" << endl << endl;
  wossInfo << "Energy: " << (station->getEnergyOnlineCount() * 100 / moduleCount) << "% "
           << station->getEnergyOnlineCount() << "/" << moduleCount << " online " << endl
           << " Production: " << station->getEnergyProduction() << " kWe  Consumption: " << station->getEnergyConsumption() << " kWe" << endl << endl;
  wossInfo << "Thermal: " << (station->getThermalOnlineCount() * 100 / moduleCount) << "% "
           << station->getThermalOnlineCount() << "/" << moduleCount << " online " << endl
           << " Production: " << station->getThermalProduction() << "kWt Consumption: " << station->getThermalConsumption() << "kWt" << endl << endl;

  wossInfo << "Discrete Events Simulation:" << endl;
  wossInfo << L"TA: " << fixed << setprecision(1) << station->getDiscreteTimeAdvanceTrend() << endl;
  if(station->isSimulationStable())
    wossInfo << L" [Stable]";
  wossInfo << endl;

  wossInfo << "DES Iterations: " << station->getDiscreteIterations() << endl;
  for(int iRes = DISCRETE_INITIAL; iRes < DISCRETE_NUM; ++iRes) {
    DiscreteResource resource = static_cast<DiscreteResource>(iRes);
    wossInfo << toString(resource) << ": " << station->getDiscreteIterationsType(resource) << endl;
  }
  wossInfo << endl;

  // ----------------------------------------
  // Module selection info

  Module* module = rBuildLayer()->getSelectedModule();
  if(NULL != module) {
    wossInfo << endl << endl << "Selected Module ";
    if(module->isOnline()) {
      wossInfo << "[Online]" << endl;
    }
    else {
      wossInfo << "[OFFLINE]" << endl;
    }
    wossInfo << "Energy: " << (module->isEnergyOnline() ? "Online" : "Offline") << endl;
    wossInfo << "Thermal: " << (module->isThermalOnline() ? "Online" : "Offline") << endl;

//     wossInfo << stringToWString(module->getName()) << " ("
//              << setprecision(1) << fixed << module->getMass() << " ton)" << endl;
    wossInfo << "Maintenance: $" << setprecision(2) << module->getPrototype()->getSimFloat(SIM_COST_MAINTENANCE) << "M/year" << endl;

    wossInfo << "LifeSupport: " << setprecision(2) << module->getLifeSupport() << L"/"
             << setprecision(2) << module->getLifeSupportBase() << endl;
    wossInfo << "Stress: " << setprecision(2) << module->getStress() << L"/"
             << setprecision(2) << module->getStressBase() << endl;
    // FIXME:
    wossInfo << "Flow: " << setprecision(1) << module->getFlowTotal() << endl;

    for(int iResource = DISCRETE_FLOW_INITIAL; iResource < DISCRETE_NUM; ++iResource) {
      DiscreteResource resource = static_cast<DiscreteResource>(iResource);
      wossInfo << toString(resource).c_str() << L" " << fixed << setprecision(2) << module->getDiscreteCurrent(resource)
               << " " << setprecision(2) << module->getDiscreteTarget(resource)
               << " (" << setprecision(1) << module->getFlowLocal(resource) << ")"
               << endl;
    }
  

//   wossInfo << "- Production: " << endl;
//   for(FlowResource type = FLOW_INITIAL; type < FLOW_NUM; ++type) {
//     if(module->getOutputCapacity(type) > 0) {
//       Real totalFlow = module->getOutputCurrent(type);
//       Real totalCapacity = module->getOutputCapacity(type);
//       Real utilization = (totalFlow / totalCapacity * 100.0f);
//       wossInfo << stringToWString(flowResourceToString(type)) << ": " << fixed
//           << setprecision(1) << totalFlow << "/" << totalCapacity << " " << flowResourceUnits(type)
//           << " "
//           << setprecision(0) << utilization << "%"
//           << endl;
//     }
//   }

//   wossInfo << "- Consumption: " << endl;
//   for(FlowResource type = FLOW_INITIAL; type < FLOW_NUM; ++type) {
//     if(module->getInputCapacity(type) > 0) {
//       Real totalFlow = module->getInputCurrent(type);
//       Real totalCapacity = module->getInputCapacity(type);
//       Real utilization = (totalFlow / totalCapacity * 100.0f);
//       wossInfo << stringToWString(flowResourceToString(type)) << ": " << fixed
//           << setprecision(1) << totalFlow << "/" << totalCapacity << " " << flowResourceUnits(type)
//           << " "
//           << setprecision(0) << utilization << "%"
//           << endl;
//     }
//   }  
  }
    
  mInfo->setLabel(wossInfo.str());
}

