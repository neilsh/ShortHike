//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2005 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "StatusReport.hpp"
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

//--------------------------------------------------------------------------------
// Singleton implementation

template<> StatusReport*
Singleton<StatusReport>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

const int MAGIC_STATUS_REPORT_WIDTH = 650;
const int MAGIC_STATUS_REPORT_HEIGHT = 600;

StatusReport::StatusReport()
  : Dialog("StatusReport"), mPrimaryObjectivesList(NULL), mSecondaryObjectivesList(NULL), mDescription(NULL), mCloseButton(NULL)
{
  setBackground(new FrameBackground());
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_STATUS_REPORT_WIDTH, MAGIC_STATUS_REPORT_HEIGHT);
  centerOnScreen();

  mPrimaryObjectivesList = new List(2, this);
  mSecondaryObjectivesList = new List(2, this);
  mPrimaryObjectivesList->setBackground(new FrameBackground());
  mSecondaryObjectivesList->setBackground(new FrameBackground());
  
  mDescription = new Label();
  mDescription->setBackground(new FrameBackground());

  mCloseButton = new Button(L"Close", this);

  GridLayout* gridLayout = new GridLayout(this, 32, 24);
  gridLayout->set_borders(3);
  gridLayout->set_padding(2);

  gridLayout->add(new Label(L"Status report"), 15, 0, 10, 1);
  gridLayout->add(new Label(L"Primary objectives"), 0, 1, 10, 1);
  gridLayout->add(mPrimaryObjectivesList, 0, 2, 32, 6);
  gridLayout->add(new Label(L"Secondary objectives"), 0, 8, 10, 1);
  gridLayout->add(mSecondaryObjectivesList, 0, 9, 32, 8);
  gridLayout->add(new Label(L"Description"), 0, 17, 32, 1);
  gridLayout->add(mDescription, 0, 18, 32, 5);
  gridLayout->add(mCloseButton, 26, 23, 6, 1);
  layout();
}

//--------------------------------------------------------------------------------

void
StatusReport::show()
{
  rMain()->getMission()->doString("ui_update_status_report()");
  Dialog::show();
}


//--------------------------------------------------------------------------------

void
StatusReport::clear()
{
  mPrimaryObjectives.clear();
  mSecondaryObjectives.clear();
  mPrimaryObjectivesList->removeAllItems();
  mSecondaryObjectivesList->removeAllItems();
}

void
StatusReport::pushObjective(wstring shortDescription, wstring longDescription, wstring progress, float progressPercentage, bool major)
{
  Objective objective;
  objective.shortDescription = shortDescription;
  objective.longDescription = longDescription;
  objective.progress = progress;
  objective.progressPercentage = progressPercentage;

  vector<wstring> objectiveEntry;
  objectiveEntry.push_back(shortDescription);
  objectiveEntry.push_back(progress);
  if(major) {
    mPrimaryObjectives.push_back(objective);
    mPrimaryObjectivesList->addItem(objectiveEntry);
  }
  else {
    mSecondaryObjectives.push_back(objective);
    mSecondaryObjectivesList->addItem(objectiveEntry);
  }
}


//--------------------------------------------------------------------------------


void
StatusReport::actionPerformed(Widget* source)
{
  if(source == mCloseButton) {
    hide();
  }
}



void
StatusReport::itemSelected(Widget* widget, unsigned num)
{
  Objective objective;
  if(widget == mPrimaryObjectivesList) {
    objective = mPrimaryObjectives[num];
  }
  else {
    objective = mSecondaryObjectives[num];
  }
  wostringstream wossDesc;
  wossDesc << objective.shortDescription << endl
           << objective.progress << " (" << fixed << objective.progressPercentage * 100 << "%)" << endl << endl
           << objective.longDescription;
  mDescription->setLabel(wossDesc.str());
}

