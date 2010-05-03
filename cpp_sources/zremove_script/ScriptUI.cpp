//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#include "Common.hpp"

#include "FunctionExporters.hpp"
#include "ScriptUtil.hpp"
#include "config/Config.hpp"
#include "config/ConfigManager.hpp"
#include "config/Player.hpp"
#include "main/Main.hpp"
#include "widgets/FileDialog.hpp"
#include "widgets/PlayerProfile.hpp"
#include "widgets/ScriptMenu.hpp"
#include "widgets/StatusReport.hpp"

//--------------------------------------------------------------------------------

int
ui_status_report_show(LuaState* state)
{
  StatusReport::getSingleton().show();
  return 0;
}

int
ui_status_report_hide(LuaState* state)
{
  StatusReport::getSingleton().hide();
  return 0;
}

int
ui_status_report_clear(LuaState* state)
{
  StatusReport::getSingleton().clear();
  return 0;
}

int
ui_status_report_push_objective(LuaState* state)
{
  wstring shortDescription = getWString(state, 1);
  wstring longDescription = getWString(state, 2);
  wstring progress = getWString(state, 3);
  if(false == state->Stack(4).IsNumber()) return 0;
  float progressPercentage = state->Stack(4).GetFloat();
  if(false == state->Stack(5).IsBoolean()) return 0;
  bool isPrimary = state->Stack(5).GetBoolean();
  StatusReport::getSingleton().pushObjective(shortDescription, longDescription, progress, progressPercentage, isPrimary);
  return 0;
}


//--------------------------------------------------------------------------------

int
ui_show_context_menu(LuaState* state)
{
  LuaObject lMenuSpec = state->Stack(1);
  if(false == lMenuSpec.IsTable()) return 0;
  int tableCount = lMenuSpec.GetCount();
  vector<ScriptMenuItem> menuItems;
  for(int index = 0; index <= tableCount; ++index) {
    LuaObject lItemSpec = lMenuSpec[index];
    if(false == lItemSpec.IsTable()) continue;

    wstring label = wstringFromLuaObject(lItemSpec["label"]);
    if(label == L"") continue;

    LuaObject lScriptCommand = lItemSpec["command"];
    if(false == lScriptCommand.IsString()) continue;
    string scriptCommand = lScriptCommand.ToString();

    LuaObject lEnabled = lItemSpec["enabled"];
    bool enabled = true;
    if(true == lEnabled.IsBoolean()) {
      enabled = lEnabled.GetBoolean();
    }
    
    ScriptMenuItem menuItem;
    menuItem.mMenuItem = label;
    menuItem.mScriptCommand = scriptCommand;
    menuItem.mIsEnabled = enabled;
    menuItems.push_back(menuItem);
  }
  ScriptMenu::getSingleton().show(menuItems);
  return 0;
}

int
ui_show_file_dialog(LuaState* state)
{
  wstring title = wstringFromLuaObject(state->Stack(1));
  wstring action = wstringFromLuaObject(state->Stack(2));
  LuaObject lFunction = state->Stack(3);
  if(false == lFunction.IsString()) return 0;
  FileDialog::getSingleton().show(title, action, lFunction.GetString());
  return 0;
}

//--------------------------------------------------------------------------------

int
ui_show_profile(LuaState* state)
{
  PlayerProfile::getSingleton().show();
  return 0;
}

int
ui_show_shop(LuaState* state)
{
  return 0;
}



//--------------------------------------------------------------------------------

void
registerUIFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();

  globals.Register("ui_status_report_show", &ui_status_report_show);
  globals.Register("ui_status_report_hide", &ui_status_report_hide);
  globals.Register("ui_status_report_clear", &ui_status_report_clear);
  globals.Register("ui_status_report_push_objective", &ui_status_report_push_objective);
  globals.Register("ui_show_context_menu", &ui_show_context_menu);
  globals.Register("ui_show_file_dialog", &ui_show_file_dialog);
  globals.Register("ui_show_profile", &ui_show_profile);
  globals.Register("ui_show_shop", &ui_show_shop);
}
