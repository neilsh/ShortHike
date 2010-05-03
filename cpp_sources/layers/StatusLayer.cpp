//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "StatusLayer.hpp"
#include "BuildLayer.hpp"

#include "game/Mission.hpp"
#include "game/Station.hpp"
#include "guilib/FlowLayout.hpp"
#include "guilib/Frame.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/Label.hpp"
#include "main/GamePlay.hpp"
#include "main/Main.hpp"
#include "rendering/RenderSystem.hpp"

// --------------------------------------------------------------------------------
// Singleton implementation

StatusLayer*
rStatusLayer()
{
  static StatusLayer* mStatusLayer = new StatusLayer();
  return mStatusLayer;
}

// --------------------------------------------------------------------------------

StatusLayer::StatusLayer()
  : mStatusFrame(NULL), mStatus(NULL)
{
  GridLayout* layout = new GridLayout(this, 32, 24);
  mStatusFrame = new Frame();
  mStatusFrame->setBackground(new FrameBackground());
  layout->add(mStatusFrame, 0, 0, 20, 1);
  mStatus = new Label(L"Station status");
  mStatusFrame->add(mStatus);
}

StatusLayer::~StatusLayer()
{
  delete mStatus;
  delete mStatusFrame;
}


//--------------------------------------------------------------------------------

void
StatusLayer::update(float)
{
  Mission* mission = rGamePlay()->getMission();
  wostringstream wossInfo;
  wossInfo << mission->getSimulationDateString() << " "
           << "AP: " << mission->getActionPointsCurrent() << "/" << mission->getActionPointsMax()
           << fixed << setprecision(0) << " $" << mission->getCredits();
  Station* station = mission->getStation();
  int moduleCount = station->getModuleCount();
  wossInfo << " Modules: " << moduleCount << " Mass: " << station->getMass() << " ton";
  float income = station->getTotalIncome();
  float expenses = station->getTotalExpenses();
  float profit = income - expenses;
  wossInfo << " Profit: " << fixed << setprecision(1) << profit
           << " Energy: " << (station->getEnergyOnlineCount() * 100 / moduleCount) << "% "
           << " Thermal: " << (station->getThermalOnlineCount() * 100 / moduleCount) << "% ";

  mStatus->setLabel(wossInfo.str());
}

