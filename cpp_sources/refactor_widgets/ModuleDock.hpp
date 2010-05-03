//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef USER_INTERFACE_MODULE_DOCK_HPP
#define USER_INTERFACE_MODULE_DOCK_HPP

#include "guilib/ActionListener.hpp"
#include "guilib/Frame.hpp"

class Module;
class GridLayout;
class RasterImage;
class Label;
class RasterImageButton;

class ModuleDock : public Widget, public ActionListener, public Singleton<ModuleDock>
{
  friend class ModuleDockLayoutManager;
public:
  ModuleDock();

  const Module* getCurrentPrototype() const;
  void updatePrototypes();

  void setActionListener(ActionListener* listener);

  virtual void actionPerformed(Widget* source);
private:
  GridLayout* iconLayout;
  Frame* iconFrame;

  const Module* currentPrototype;
  ActionListener* listener;

  int moduleWidth;
  int moduleHeight;
  map<Widget*, const Module*> modules;
  map<string, RasterImageButton*> mModuleButtons;
};



#endif
