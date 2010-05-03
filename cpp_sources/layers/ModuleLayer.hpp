//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef LAYERS_MODULE_LAYER_HPP
#define LAYERS_MODULE_LAYER_HPP

#include "guilib/Widget.hpp"
#include "guilib/ActionListener.hpp"

class Mission;
class Frame;
class Label;
class RasterImageButton;
class Prototype;

class ModuleLayer : public Widget, public ActionListener
{
public:
  ModuleLayer();
  virtual ~ModuleLayer();

  void setMission(Mission* mission);
  void updateModules();

  virtual void actionPerformed(Widget* source);
private:
  Mission* mMission;

  Frame* mModuleFrame;
  map<RasterImageButton*, Prototype*> mModuleButtons;
};


#endif
