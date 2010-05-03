//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef WIDGETS_LAYER_DOCK_HPP
#define WIDGETS_LAYER_DOCK_HPP

#include "guilib/ActionListener.hpp"
#include "guilib/Frame.hpp"

class Label;
class Button;
class ScriptButton;
class Orbit;
class OrbitMap;

class LayerDock : public Frame, public ActionListener
{
public:
  LayerDock(Orbit* orbitScreen, OrbitMap* orbitMap);

  virtual void actionPerformed(Widget* source);
  virtual void update(Time);

private:
  void setMarkerVisible(bool visible);

  Orbit* orbitScreen;

  Label* layerInfo;

  wstring layerName;

  Button* visualButton;
  Button* statusButton;
  ScriptButton* mMessagesButton;
  ScriptButton* mShopButton;
};



#endif
