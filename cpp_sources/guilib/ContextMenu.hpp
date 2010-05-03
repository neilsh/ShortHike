//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef GUILIB_CONTEXT_MENU_HPP
#define GUILIB_CONTEXT_MENU_HPP

class Widget;
class List;

#pragma warning(disable: 4100)

class ContextMenuListener
{
public:
  virtual void contextItemSelected(Widget* source, unsigned itemNum, wstring label) = 0;
  virtual void contextItemDeSelected(Widget* source, unsigned itemNum, wstring label) {};
};


#include "Dialog.hpp"


class ContextMenu : public Dialog
{
  friend class ContextMenuProxy;
public:
  ContextMenu(string name, ContextMenuListener* listener = NULL, vector<wstring> menuItems = vector<wstring>());

  void setListener(ContextMenuListener* _listener) {listener = _listener;}
  
  void setMenuItems(vector<wstring> menuItems);
  void setMenuItems(vector<wstring> menuItems, vector<bool> areItemsEnabled);
  void showMenu(Point location);
  void showMenu();
  void hideMenu();
  
  virtual bool keyboardEvent(char keyCode, bool pressed);
  virtual void mouseClickOutsideBounds();
private:
  void initialize(ContextMenuListener* listener);
  void fireMessage(unsigned selected);
  
  ContextMenuListener* listener;
  Point clickLocation;
  GRect menuArea;
  
  ContextMenuProxy* proxy;
  
  //  ImageCanvas* menu_pane;
  List* menuList;
};


#endif
