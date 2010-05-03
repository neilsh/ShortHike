//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef WIDGETS_MISSION_SELECTOR_HPP
#define WIDGETS_MISSION_SELECTOR_HPP

#include "guilib/ActionListener.hpp"
#include "guilib/Dialog.hpp"
#include "guilib/List.hpp"

class List;
class Label;
class Button;
class Campaign;

class MissionSelector : public Dialog, public ActionListener, public ListListener, public Singleton<MissionSelector>
{
public:
  MissionSelector();

  virtual void show(bool enableClose = true);
  
  void clearMissions();
  void addCampaign(string handle, wstring name, int order);
  void addCampaignGroup(string handle, vector<string> missions);
  void addMission(string handle, wstring name, wstring description, string scriptfile, int order);

  void startNextMission();
  void replayMission();
  void loadMission(string fileName);

  virtual void actionPerformed(Widget* source);
  virtual void itemSelected(Widget* source, unsigned num);

  struct Entry 
  {
    string handle;
    wstring name;
    wstring description;
    string script;
    int order;
  };
private:
  void startMission(unsigned num);
  void updateMissionList();
  
  Campaign* mCurrentCampaign;
  int mMissionNum;
  
  List* mCampaignList;
  Label* mCampaignDesc;

  List* mMissionList;
  Label* mMissionDesc;

  Button* mHomeBaseButton;
  Button* mStartButton;
  Button* mCloseButton;

  vector<Campaign*> mCampaigns;
  map<string, Campaign*> mHandleToCampaign;

  vector<Entry> mMissions;
  vector<Entry> mCurrentMissions;
};



#endif
