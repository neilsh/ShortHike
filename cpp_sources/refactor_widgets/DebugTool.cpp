//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2005 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "DebugTool.hpp"

#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "game/Station.hpp"
#include "guilib/Button.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/Label.hpp"
#include "main/Main.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> DebugTool*
Singleton<DebugTool>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

const int MAGIC_DEBUG_TOOL_WIDTH = 200;
const int MAGIC_DEBUG_TOOL_HEIGHT = 400;

DebugTool::DebugTool()
  : Frame("DebugTool"), mDebug(NULL)
{
  setBackground(new FrameBackground());
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_DEBUG_TOOL_WIDTH, MAGIC_DEBUG_TOOL_HEIGHT);
  setPosition(screenSize.w - MAGIC_DEBUG_TOOL_WIDTH, 100);

  mDebug = new Label(L"Debug");

  GridLayout* gridLayout = new GridLayout(this, 10, 20);
  gridLayout->set_borders(3);
  gridLayout->set_padding(2);

  gridLayout->add(new Label(L"Debug Tool"), 0, 0, 10, 1);
  gridLayout->add(mDebug, 0, 1, 10, 18);

  layout();
  setVisible(false);
  rGUIManager()->getRootContainer()->add(this);
}

//--------------------------------------------------------------------------------
// The show method handles all end of mission bookkeeping

void
DebugTool::update(Time)
{
  Station* station = rMain()->getMission()->getStation();
  wostringstream wossDebug;
  wossDebug << L"TA: " << fixed << setprecision(1) << station->getDiscreteTimeAdvanceTrend() << endl;
  if(station->isSimulationStable())
    wossDebug << L" [Stable]";
  wossDebug << endl;

  wossDebug << "DES Iterations: " << station->getDiscreteIterations() << endl;
  for(int iRes = DISCRETE_INITIAL; iRes < DISCRETE_NUM; ++iRes) {
    DiscreteResource resource = static_cast<DiscreteResource>(iRes);
    wossDebug << toString(resource) << L": " << station->getDiscreteIterationsType(resource) << endl;
  }

  mDebug->setLabel(wossDebug.str());
}

