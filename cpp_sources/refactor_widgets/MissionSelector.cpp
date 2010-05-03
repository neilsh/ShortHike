//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2005 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "MissionSelector.hpp"
#include "config/Config.hpp"
#include "config/ConfigManager.hpp"
#include "config/Player.hpp"
#include "game/Campaign.hpp"
#include "game/Mission.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/List.hpp"
#include "guilib/Label.hpp"
#include "guilib/Button.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "main/Main.hpp"
#include "orbit/Orbit.hpp"
#include "property/DataFile.hpp"
#include "widgets/ScriptButton.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> MissionSelector*
Singleton<MissionSelector>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

const int MAGIC_MISSION_SELECTOR_WIDTH = 650;
const int MAGIC_MISSION_SELECTOR_HEIGHT = 600;

MissionSelector::MissionSelector()
  : Dialog("MissionSelector"),
    mCampaignList(NULL), mCampaignDesc(NULL),
    mMissionList(NULL), mMissionDesc(NULL),
    mHomeBaseButton(NULL), mStartButton(NULL), mCloseButton(NULL),
    mMissionNum(0), mCurrentCampaign(NULL)
{
  setBackground(new FrameBackground());
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_MISSION_SELECTOR_WIDTH, MAGIC_MISSION_SELECTOR_HEIGHT);
  centerOnScreen();

  mCampaignList = new List(1, this);
  mCampaignList->setBackground(new FrameBackground());
  mCampaignDesc = new Label();
  mCampaignDesc->setBackground(new FrameBackground());

  mMissionList = new List(1, this);
  mMissionList->setBackground(new FrameBackground());  
  mMissionDesc = new Label();
  mMissionDesc->setBackground(new FrameBackground());

  mHomeBaseButton = new Button(L"Home Base", this);
  mStartButton = new Button(L"Start Mission", this);
  mCloseButton = new Button(L"Close", this);

  GridLayout* gridLayout = new GridLayout(this, 32, 24);
  gridLayout->set_borders(3);
  gridLayout->set_padding(2);

  gridLayout->add(new Label(L"Missions available - Select Campaign"), 0, 0, 26, 1);

  gridLayout->add(mCampaignList, 0, 1, 14, 7);
  gridLayout->add(mCampaignDesc, 14, 1, 18, 7);
    
  gridLayout->add(new Label(L"Available missions"), 0, 8, 26, 1);
  gridLayout->add(mMissionList, 0, 9, 14, 14);
  gridLayout->add(mMissionDesc, 14, 9, 18, 14);

  gridLayout->add(mHomeBaseButton, 6, 23, 6, 1);
  gridLayout->add(mStartButton, 0, 23, 6, 1);
  gridLayout->add(mCloseButton, 26, 23, 6, 1);
  layout();
}

//--------------------------------------------------------------------------------

bool
lessEntry(MissionSelector::Entry entry1, MissionSelector::Entry entry2)
{
  return entry1.order < entry2.order;
}

bool
lessCampaign(Campaign* campaign1, Campaign* campaign2)
{
  return campaign1->getOrder() < campaign2->getOrder();
}



void
MissionSelector::show(bool enableClose)
{
  mCloseButton->setEnabled(enableClose);
  
  Mission* currentMission = rMain()->getMission();
  if(ConfigManager::getPlayer()->getHomeBase() == currentMission) {
    mHomeBaseButton->setVisible(false);
  }
  else {
    mHomeBaseButton->setVisible(true);
  }  

  sort(mCampaigns.begin(), mCampaigns.end(), lessCampaign);
  mCampaignList->removeAllItems();  
  for(vector<Campaign*>::const_iterator iCampaign = mCampaigns.begin(); iCampaign != mCampaigns.end(); ++iCampaign) {
    Campaign* campaign = *iCampaign;
    mCampaignList->addItem(campaign->getName());
  }  

  sort(mMissions.begin(), mMissions.end(), lessEntry);
  updateMissionList();
  
  Dialog::show();
}


void
MissionSelector::updateMissionList()
{
  mCurrentMissions.clear();
  mMissionList->removeAllItems();
  if(mCurrentCampaign != NULL) {
    for(vector<Entry>::const_iterator iMission = mMissions.begin(); iMission != mMissions.end(); ++iMission) {
      Entry mission = *iMission;
      if(mCurrentCampaign->hasMission(mission.handle)) {
        mCurrentMissions.push_back(mission);
        mMissionList->addItem(mission.name);
      }
    }
  }
}



//--------------------------------------------------------------------------------

void
MissionSelector::clearMissions()
{
  mMissions.clear();
}

void
MissionSelector::addCampaign(string handle, wstring name, int order)
{
  Campaign* campaign = new Campaign(handle, name, order);
  mCampaigns.push_back(campaign);
  mHandleToCampaign[handle] = campaign;
}

void
MissionSelector::addCampaignGroup(string handle, vector<string> missions)
{
  Campaign* campaign = mHandleToCampaign[handle];
  if(campaign == NULL) return;
  campaign->addGroup(missions);
}

void
MissionSelector::addMission(string handle, wstring name, wstring description, string script, int order)
{
  Entry mission;
  mission.handle = handle;
  mission.name = name;
  mission.description = description;
  mission.script = script;
  mission.order = order;
  mMissions.push_back(mission);
}


//--------------------------------------------------------------------------------


void
MissionSelector::actionPerformed(Widget* source)
{
  if(source == mCloseButton) {
    hide();
  }
  else if(source == mStartButton) {
    hide();
    int num = mMissionList->getSelected();
    startMission(num);
  }  
  else if(source == mHomeBaseButton) {
    hide();
    rMain()->setMission(ConfigManager::getPlayer()->getHomeBase());
  }  
}

void
MissionSelector::startNextMission()
{
  startMission(0);
}

void
MissionSelector::replayMission()
{
  startMission(mMissionNum);
}


void
MissionSelector::startMission(unsigned num)
{
  Entry missionEntry;
  if(mCurrentCampaign == NULL) {
    // FIXME: we should fall back to station school in case a first timer plays
    missionEntry = mMissions[0];
  }
  else {
    if(num < 0 || num >= static_cast<int>(mCurrentMissions.size())) return;
    mMissionNum = num;
    missionEntry = mCurrentMissions[num];
  }
  
  Mission* oldMission = rMain()->getMission();
  
  Orbit* orbitScreen = static_cast<Orbit*>(rMain()->getScreenManager().getState(Main::ORBIT_SCREEN));
  orbitScreen->enterReadyState();
  
  DataFile* dataFile = new DataFile();
  Mission* mission = new Mission(dataFile);
  dataFile->setRootItem(mission);
  rMain()->setMission(mission);
  mission->initScriptFile(missionEntry.script);
  
  if(oldMission != NULL) {
    if (oldMission == ConfigManager::getPlayer()->getHomeBase()) {
      ConfigManager::getSingleton().savePlayer();
    }
    else {
      delete oldMission->getDataFile();
    }
  }
}

void
MissionSelector::loadMission(string fileName)
{
  if(fileName == "") return;

  Mission* oldMission = rMain()->getMission();
  
  Orbit* orbitScreen = static_cast<Orbit*>(rMain()->getScreenManager().getState(Main::ORBIT_SCREEN));
  orbitScreen->enterReadyState();
  
  DataFile* dataFile = new DataFile(stringToWString(fileName));

  if(dataFile->getRootItem() != NULL) {
    Mission* mission = dataFile->getRootItem()->castToClass<Mission>();
    if(mission != NULL) {
      rMain()->setMission(mission);      
  
      if(oldMission != NULL) {
        if (oldMission == ConfigManager::getPlayer()->getHomeBase()) {
          ConfigManager::getSingleton().savePlayer();
        }
        else {
          delete oldMission->getDataFile();
        }
      }
    }
  }
}


//--------------------------------------------------------------------------------

void
MissionSelector::itemSelected(Widget* source, unsigned num)
{
  if(source == mCampaignList) {
    mCurrentCampaign = mCampaigns[num];
    updateMissionList();
    wostringstream wossDesc;
    wossDesc << L"Name: " << mCurrentCampaign->getName();
    mCampaignDesc->setLabel(wossDesc.str());
  }
  else if(source == mMissionList) {
    const Entry& mission = mCurrentMissions[num];
    wostringstream wossDesc;
    wossDesc << L"Name: " << mission.name << endl << endl << "Description:" << endl << mission.description;
    mMissionDesc->setLabel(wossDesc.str());
  }  
}

