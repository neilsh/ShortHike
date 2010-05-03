//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2005 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "Debrief.hpp"
#include "config/ConfigManager.hpp"
#include "config/Player.hpp"
#include "game/Mission.hpp"
#include "guilib/Button.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/Label.hpp"
#include "main/Main.hpp"
#include "widgets/MissionSelector.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> Debrief*
Singleton<Debrief>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

const int MAGIC_DEBRIEF_WIDTH = 650;
const int MAGIC_DEBRIEF_HEIGHT = 600;

Debrief::Debrief()
  : Dialog("Debrief"), mDebriefing(NULL), mNextMission(NULL), mReplay(NULL), mHomeBase(NULL), mMissionSelector(NULL)
{
  setBackground(new FrameBackground());
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_DEBRIEF_WIDTH, MAGIC_DEBRIEF_HEIGHT);
  centerOnScreen();

  mDebriefing = new Label(L"DEBRIEFING");

  mNextMission = new Button(L"Next Mission", this);
  mReplay = new Button(L"Replay Mission", this);
  mHomeBase = new Button(L"Home Base", this);
  mMissionSelector = new Button(L"Missions", this);

  GridLayout* gridLayout = new GridLayout(this, 32, 24);
  gridLayout->set_borders(3);
  gridLayout->set_padding(2);

  gridLayout->add(new Label(L"Mission Complete: Debriefing"), 0, 0, 26, 1);

  gridLayout->add(mDebriefing, 0, 1, 32, 22);

  gridLayout->add(mMissionSelector, 0, 23, 6, 1);
  gridLayout->add(mHomeBase, 6, 23, 6, 1);
  gridLayout->add(mReplay, 20, 23, 6, 1);
  gridLayout->add(mNextMission, 26, 23, 6, 1);

  layout();
}

//--------------------------------------------------------------------------------
// The show method handles all end of mission bookkeeping

void
Debrief::show()
{
  lua_State* cLuaState = rMain()->getMission()->getLuaState()->GetCState();
  LuaAutoBlock autoBlock(cLuaState);
  lua_getglobal(cLuaState, "trigger_get_bonus_summary");
  if(lua_pcall(cLuaState, 0, 2, 0)) {
    logGameInfo(string("Lua: ") + lua_tostring(cLuaState, -1));
    mDebriefing->setLabel(L"No debrief provided");
  }
  else {
    if(!lua_isstring(cLuaState, -2)) return;
    if(!lua_isnumber(cLuaState, -1)) return;
    float bonusAmount = lua_tonumber(cLuaState, -1);
    wostringstream wossDebriefMessage;
    wossDebriefMessage << L"Station manager bonus breakdown:" << endl
                       << stringToWString(lua_tostring(cLuaState, -2)) << endl
                       << endl;
    if(bonusAmount < 1.5)
      wossDebriefMessage << L"Total: $" << bonusAmount * 1000 << "k";
    else
      wossDebriefMessage << L"Total: $" << bonusAmount << "M";
    mDebriefing->setLabel(wossDebriefMessage.str());
    ConfigManager::getHomeBase()->creditTransaction(bonusAmount);

    Player* player = ConfigManager::getPlayer();
    player->mExperience += bonusAmount;
    if(player->mLargestBonus < bonusAmount) {
      player->mLargestBonus = bonusAmount;
      player->mLargestBonusMission = rMain()->getMission()->getName();
    }      

    ConfigManager::getSingleton().savePlayer();
  }  

  Dialog::show();
}


//--------------------------------------------------------------------------------


void
Debrief::actionPerformed(Widget* source)
{
  if(source == mHomeBase) {
    hide();
    rMain()->setMission(ConfigManager::getHomeBase());
  }  
  else if(source == mReplay) {
    hide();
    MissionSelector::getSingleton().replayMission();
  }
  else if(source == mNextMission) {
    hide();
    MissionSelector::getSingleton().startNextMission();
  }
  else if(source == mMissionSelector) {
    hide();
    MissionSelector::getSingleton().show(false);
  }  
}



