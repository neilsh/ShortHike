//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "ModuleDock.hpp"
#include "game/Mission.hpp"
#include "game/Module.hpp"
#include "guilib/Frame.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/Graphics.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/Label.hpp"
#include "guilib/RasterImage.hpp"
#include "guilib/RasterImageButton.hpp"
#include "guilib/Widget.hpp"
#include "main/Main.hpp"
#include "property/PrototypeManager.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> ModuleDock*
Singleton<ModuleDock>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

class ModuleDockLayoutManager : public LayoutManager
{
  virtual void doLayout(Widget *c)
  {
    ModuleDock* dock = static_cast<ModuleDock*>(c);
    int dockHeight = dock->getHeight();
    int dockWidth = dockHeight / dock->moduleHeight * dock->moduleWidth;
    dock->iconFrame->setPosition(0, 0); 
    dock->iconFrame->setSize(dockWidth, dockHeight);
  }
  
  virtual Dimension preferredLayoutSize(Widget *c)
  {
    return c->getPreferredSize();
  }
};


//--------------------------------------------------------------------------------

ModuleDock::ModuleDock()
  : currentPrototype(NULL), listener(NULL), iconLayout(NULL), iconFrame(NULL)
{
  iconFrame = new Frame("ModuleDock/IconFrame");
  iconFrame->setBackground(new FrameBackground());
  add(iconFrame);
  setLayoutManager(new ModuleDockLayoutManager());
  
  moduleWidth = 1;
  moduleHeight = rGUIManager()->getScreenHeight() / 64;
  
  updatePrototypes();
}


void
ModuleDock::actionPerformed(Widget* source)
{
  currentPrototype = modules[source];
  if(listener != NULL)
    listener->actionPerformed(this);
}


const Module*
ModuleDock::getCurrentPrototype() const
{
  return currentPrototype;
}

void
ModuleDock::setActionListener(ActionListener* _listener)
{
  listener = _listener;
}


//--------------------------------------------------------------------------------

void
ModuleDock::updatePrototypes()
{
  if(NULL == rMain()->getMission()) return;
  map<string, Module*> prototypes = PrototypeManager::getSingleton().getPrototypes<Module>();
  set<string> enabledHandles = rMain()->getMission()->getEnabledModules();
  vector<Module*> enabledPrototypes;
  for(map<string, Module*>::const_iterator iPrototype = prototypes.begin(); iPrototype != prototypes.end(); ++iPrototype) {
    string handle = (*iPrototype).first;
    Module* module = (*iPrototype).second;
    if(enabledHandles.find(handle) != enabledHandles.end()) {
      enabledPrototypes.push_back(module);
    }    
  }

  moduleWidth = (enabledPrototypes.size() / moduleHeight) + 1;

  if(iconLayout != NULL) delete iconLayout;

  iconLayout = new GridLayout(iconFrame, moduleWidth, moduleHeight);
  iconLayout->set_borders(3);
  iconLayout->set_padding(2);  

  int xpos = 0;
  int ypos = 0;
  iconFrame->removeAll();
  for(vector<Module*>::const_iterator prototypeI = enabledPrototypes.begin(); prototypeI != enabledPrototypes.end(); ++prototypeI) {
    const Module* prototype = *prototypeI;

    RasterImageButton* button = mModuleButtons[prototype->getIdentifier()];
    if(NULL == button) {
      button = new RasterImageButton(prototype->getIconName());
      mModuleButtons[prototype->getIdentifier()] = button;
    }

    modules[button] = prototype;
    button->addActionListener(this);
    iconLayout->add(button, xpos, ypos);
    xpos++;
    if(xpos == iconLayout->getLayoutWidth()) {
      xpos = 0;
      ypos++;      
    }
    if(ypos == iconLayout->getLayoutHeight())
      break;
  }
  iconFrame->setVisible(0 < enabledPrototypes.size());
  layout();
}
