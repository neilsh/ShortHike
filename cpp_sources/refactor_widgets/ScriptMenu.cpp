//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "ScriptMenu.hpp"
#include "game/Mission.hpp"
#include "main/Main.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> ScriptMenu*
Singleton<ScriptMenu>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

const int MAGIC_FILE_DIALOG_SIZE = 512;

ScriptMenu::ScriptMenu()
  : ContextMenu("ScriptMenu")
{
  setListener(this);
}

//--------------------------------------------------------------------------------

void
ScriptMenu::show(vector<ScriptMenuItem> menuItems)
{
  mMenuItems = menuItems;
  vector<wstring> labels;
  vector<bool> isEnabled;
  for(vector<ScriptMenuItem>::const_iterator iItem = menuItems.begin(); iItem != menuItems.end(); ++iItem) {
    ScriptMenuItem item = *iItem;
    labels.push_back(item.mMenuItem);
    isEnabled.push_back(item.mIsEnabled);
  }
  setMenuItems(labels, isEnabled);
  showMenu();
}


//--------------------------------------------------------------------------------

void
ScriptMenu::contextItemSelected(Widget* , unsigned itemNum, wstring )
{
  rMain()->getMission()->doString(mMenuItems[itemNum].mScriptCommand);
}
