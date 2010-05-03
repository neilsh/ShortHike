//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "ModuleLayer.hpp"
#include "BuildLayer.hpp"

#include "game/Mission.hpp"
#include "game/Prototype.hpp"
#include "guilib/FlowLayout.hpp"
#include "guilib/Frame.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/Label.hpp"
#include "guilib/RasterImageButton.hpp"
#include "main/Main.hpp"
#include "rendering/RenderSystem.hpp"

// --------------------------------------------------------------------------------
// Singleton implementation

ModuleLayer*
rModuleLayer()
{
  static ModuleLayer* mModuleLayer = new ModuleLayer();
  return mModuleLayer;
}

// --------------------------------------------------------------------------------

ModuleLayer::ModuleLayer()
  : mMission(NULL), mModuleFrame(NULL)
{
  GridLayout* layout = new GridLayout(this, 32, 24);
  mModuleFrame = new Frame();
  layout->add(mModuleFrame, 0, 22, 32, 2);
  mModuleFrame->setBackground(new FrameBackground());
  mModuleFrame->setLayoutManager(new FlowLayout(FlowLayout::HORIZONTAL));
}

ModuleLayer::~ModuleLayer()
{
}

// --------------------------------------------------------------------------------

void
ModuleLayer::setMission(Mission* mission)
{
  mMission = mission;
  updateModules();
}

//--------------------------------------------------------------------------------

void
ModuleLayer::updateModules()
{
  for(map<RasterImageButton*, Prototype*>::iterator iButton = mModuleButtons.begin(); iButton != mModuleButtons.end(); ++iButton)
    delete (*iButton).first;

  mModuleFrame->removeAll();
  if(NULL != mMission) {
    const vector<Prototype*>& prototypes = mMission->getPrototypes();
    for(vector<Prototype*>::const_iterator iPrototype = prototypes.begin(); iPrototype != prototypes.end(); ++iPrototype) {
      Prototype* prototype = *iPrototype;
      RasterImageButton* button = new RasterImageButton(prototype->getIconSurface48());
      button->addActionListener(this);
      mModuleButtons[button] = *iPrototype;
      mModuleFrame->add(button);
    }
  }
  else {
    mModuleButtons.clear();
  }
  layout();
}

// --------------------------------------------------------------------------------

void
ModuleLayer::actionPerformed(Widget* source)
{
  Prototype* prototype = mModuleButtons[static_cast<RasterImageButton*>(source)];
  if(NULL != prototype)
    rBuildLayer()->setAddState(prototype);
}
