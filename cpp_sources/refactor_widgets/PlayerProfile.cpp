//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2005 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "PlayerProfile.hpp"
#include "config/ConfigManager.hpp"
#include "config/Player.hpp"
#include "guilib/Button.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/Label.hpp"


//--------------------------------------------------------------------------------
// Singleton implementation

template<> PlayerProfile*
Singleton<PlayerProfile>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

const int MAGIC_PROFILE_WIDTH = 650;
const int MAGIC_PROFILE_HEIGHT = 600;

PlayerProfile::PlayerProfile()
  : Dialog("PlayerProfile"), mHeading(NULL), mProfile(NULL), mCloseButton(NULL)
{
  setBackground(new FrameBackground());
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_PROFILE_WIDTH, MAGIC_PROFILE_HEIGHT);
  centerOnScreen();

  mHeading = new Label(L"Incognito's profile");
  mProfile = new Label(L"PROFILE");
  mCloseButton = new Button(L"Ok", this);

  GridLayout* gridLayout = new GridLayout(this, 32, 24);
  gridLayout->set_borders(3);
  gridLayout->set_padding(2);

  gridLayout->add(mHeading, 0, 0, 26, 1);
  gridLayout->add(mProfile, 0, 1, 32, 22);
  gridLayout->add(mCloseButton, 26, 23, 6, 1);

  layout();
}

//--------------------------------------------------------------------------------

void
PlayerProfile::show()
{
  Player* player = ConfigManager::getPlayer();
  wostringstream wossProfile;
  wossProfile << endl;
  int totalTimeMinutes = static_cast<int>(player->mPlayTime / 60);
  int timeMinutes = totalTimeMinutes % 60;
  int totalTimeHours = totalTimeMinutes / 60;
  wossProfile << L"Total time: " << totalTimeHours << L"h " << timeMinutes << L"min" << endl << endl;
  
  wossProfile << L"Bonus earned: $" << fixed << setprecision(1);
  if(player->mExperience < 2)
    wossProfile << player->mExperience / 1000 << "k" << endl;
  else
    wossProfile << player->mExperience << L"M" << endl;
  
  wossProfile << L"Largest bonus: $" << fixed << setprecision(1);
  if(player->mLargestBonus < 2)
    wossProfile << player->mLargestBonus / 1000 << L"k";
  else
    wossProfile << player->mLargestBonus << L"M";
  wossProfile << L" (" << player->mLargestBonusMission << L")" << endl << endl;

  wossProfile << L"Modules added: " << player->mModulesAdded << endl
              << L"Modules moved: " << player->mModulesMoved << endl
              << L"Modules deleted: " << player->mModulesDeleted << endl
              << L"Revolutions rotated: " << fixed << setprecision(1) << player->mRevolutionsRotated << endl
              << L"Assemblies created: " << player->mAssembliesCreated << endl << endl;
  mProfile->setLabel(wossProfile.str());
  mHeading->setLabel(player->getName() + L"'s profile");
  Dialog::show();
}


//--------------------------------------------------------------------------------


void
PlayerProfile::actionPerformed(Widget* source)
{
  if(source == mCloseButton) {
    hide();
  }  
}



