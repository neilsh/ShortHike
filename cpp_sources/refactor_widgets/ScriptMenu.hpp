//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef USER_INTERFACE_SCRIPT_MENU
#define USER_INTERFACE_SCRIPT_MENU

#include "guilib/ContextMenu.hpp"

struct ScriptMenuItem
{
  wstring mMenuItem;
  string mScriptCommand;
  bool mIsEnabled;
};

class ScriptMenu : public ContextMenu, public Singleton<ScriptMenu>, public ContextMenuListener
{
public:

  ScriptMenu();  

  void show(vector<ScriptMenuItem> menuItems);

  virtual void contextItemSelected(Widget* source, unsigned itemNum, wstring label);
private:
  vector<ScriptMenuItem> mMenuItems;
};


#endif
