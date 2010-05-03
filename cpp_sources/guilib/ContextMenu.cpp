//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2003 Kai-Peter Baeckman, Mistaril Ltd.
// www.mistaril.com
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "List.hpp"
#include "ContextMenu.hpp"
#include "FlowLayout.hpp"
#include "GUIManager.hpp"
#include "FrameBackground.hpp"

class ContextMenuProxy : public ListListener
{
public:
  ContextMenuProxy(ContextMenu* hostParameter)
  {
    host = hostParameter;
  }
  
  virtual void itemSelected(Widget* , unsigned num)
  {
    host->fireMessage(num);
  }
private:
  ContextMenu* host;
};



ContextMenu::ContextMenu(string name, ContextMenuListener* _listener, vector<wstring> menuItems)
  : Dialog()
{
  proxy = new ContextMenuProxy(this);
  menuList = new List(1, proxy);
  listener = _listener;

  setLayoutManager(NULL);
  setBackground(new FrameBackground());
  
//   menu_pane = new ImageCanvas("assets/ssm/graphics/faded_background.png", 3);
//   FlowLayout* pane_layout = new FlowLayout(FlowLayout::VERTICAL, FlowLayout::CENTER_HORIZONTALLY, FlowLayout::CENTER_VERTICALLY);
//   pane_layout->set_borders(2);
//   pane_layout->set_padding(1);
//   setLayoutManager(pane_layout);
//   menu_pane->add(menuList, "CENTER");
  
  add(menuList);
  setMenuItems(menuItems);
}


void
ContextMenu::setMenuItems(vector<wstring> menuItems)
{
  menuList->removeAllItems();
  for(vector<wstring>::const_iterator menuI = menuItems.begin(); menuI != menuItems.end(); ++menuI) {
    wstring item = *menuI;
    menuList->addItem(item);
  }
  markDirty();
}


void
ContextMenu::setMenuItems(vector<wstring> menuItems, vector<bool> areItemsEnabled)
{
  CHECK("Item and enable information count is equal", menuItems.size() == areItemsEnabled.size());

  menuList->removeAllItems();
  for(unsigned itemI = 0; itemI < menuItems.size(); ++itemI) {
    wstring item = menuItems[itemI];
    bool enabled = areItemsEnabled[itemI];
    menuList->addItem(item, enabled);
  }
  markDirty();
}


//--------------------------------------------------------------------------------


bool
ContextMenu::keyboardEvent(char keyCode, bool pressed)
{
  if(keyCode == VK_ESCAPE || keyCode == VK_SPACE) {
    // REFACTOR: Change to Layer based
//     rGUIManager()->popAllDialogs();
    return true;
  }
  return false;
}


void
ContextMenu::mouseClickOutsideBounds()
{
  hideMenu();
}

//--------------------------------------------------------------------------------

void
ContextMenu::showMenu()
{
  showMenu(rGUIManager()->getCursorPosition());
}



void
ContextMenu::showMenu(Point location)
{
  if(menuList->size() == 0) return;

  // REFACTOR: Change to Layer based
//   rGUIManager()->pushDialog(this);
  clickLocation = location;

  menuList->setSize(menuList->getPreferredSize());
  menuList->setPosition(0, 0);

  menuArea;
  menuArea.setSize(menuList->getPreferredSize());
  menuArea.setLocation(clickLocation);

  Dimension screenSize = rGUIManager()->getScreenDimension();
  if((menuArea.x + menuArea.w) > screenSize.w) {
    menuArea.x -= menuArea.w;
  }
  if((menuArea.y + menuArea.h) > screenSize.h) {
    menuArea.y -= menuArea.h;
  }

  setPosition(menuArea.x, menuArea.y);
  setSize(menuArea.w, menuArea.h);

  menuList->resetPointed();
  markDirty();
}


void
ContextMenu::hideMenu()
{
  // Change to Layer based
//   rGUIManager()->popDialog();
}

void
ContextMenu::fireMessage(unsigned selected)
{
  hideMenu();
  if(listener != NULL)
    listener->contextItemSelected(this, selected, menuList->getItem(selected));
}


