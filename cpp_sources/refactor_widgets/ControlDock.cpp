//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "ControlDock.hpp"
#include "version.hpp"
#include "config/ConfigManager.hpp"
#include "config/Config.hpp"
#include "drm/License.hpp"
#include "game/Aspects.hpp"
#include "game/Location.hpp"
#include "game/Mission.hpp"
#include "game/Station.hpp"
#include "guilib/BorderLayout.hpp"
#include "guilib/Button.hpp"
#include "guilib/Frame.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/Label.hpp"
#include "main/Main.hpp"
#include "orbit/Orbit.hpp"
#include "util/State.hpp"
#include "widgets/ScriptButton.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> ControlDock*
Singleton<ControlDock>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

ControlDock::ControlDock()
  : currentStation(NULL), gridLayout(NULL), infoFrame(NULL), infoLabel(NULL),
    menuFrame(NULL)
{
  GridLayout* controlLayout = new GridLayout(this, 24, 2);  
  controlLayout->set_padding(0);
  controlLayout->set_borders(0);
  
  {
    infoFrame = new Frame("ControlDock/InfoFrame");
    infoFrame->setBackground(new FrameBackground());
    GridLayout* infoLayout = new GridLayout(infoFrame, 24, 1);
    infoLayout->set_padding(2);
    infoLayout->set_borders(2);

    infoLabel = new Label(L"Corporate information");
    mTrialTimer = new Label();
    mBuyButton = new ScriptButton(stringToWString(BUILD_BUY_NOW), "open_buy_page()", 14);

    infoLayout->add(infoLabel, 0, 0, 16, 1);
    infoLayout->add(mTrialTimer, 16, 0, 4, 1);
    infoLayout->add(mBuyButton, 20, 0, 4, 1);    
  }

  {
    menuFrame = new Frame("ControlDock/MenuFrame");
    menuFrame->setBackground(new FrameBackground());

    GridLayout* menuLayout = new GridLayout(menuFrame, 8, 1);
    menuLayout->set_padding(0);
    menuLayout->set_borders(0);
  }  


  controlLayout->add(infoFrame, 0, 1, 24, 1);
}


//--------------------------------------------------------------------------------

void
ControlDock::update(Time)
{
  wostringstream infoStream;
  Mission* mission = rMain()->getMission();

  infoStream << mission->getSimulationDateString() << L" AP:"
             << mission->getActionPointsCurrent() << L"/" << mission->getActionPointsMax() << L" ";

  if(rMain()->getAspects()->isCredits()) {
    infoStream << L"$" << fixed << setprecision(1) << mission->getCredits() << L"M - ";
  }  
  
  if(currentStation != NULL) {
    infoStream << currentStation->getName();
//     Location* location = currentStation->getLocation();
//  << " ["
//                << location->getName() << "]";
  }
  infoLabel->setLabel(infoStream.str());

  License* currentLicense = ConfigManager::getCurrent()->getLicense();
  if(currentLicense->isActive()) {
    mTrialTimer->setVisible(false);
    mBuyButton->setVisible(false);
  }
  else {
    mTrialTimer->setVisible(true);
    mBuyButton->setVisible(true);

    wostringstream wossLicenseInfo;
    wossLicenseInfo << fixed << setprecision(0) << currentLicense->getTrialMinutes() << " minutes";
    mTrialTimer->setLabel(wossLicenseInfo.str());
  }
}

//--------------------------------------------------------------------------------

void
ControlDock::actionPerformed(Widget* source)
{
}

