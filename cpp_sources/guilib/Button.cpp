//--------------------------------------------------------------------------------
//
// ShortHike - www.shorthike.com
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Button.hpp"
#include "ActionListener.hpp"
#include "ButtonListener.hpp"
#include "Graphics.hpp"
#include "GUIManager.hpp"
#include "GRect.hpp"
//#include "audio/SoundManager.hpp"


// -------------------------------------------------------------------------------- 
 
Button::Button(wstring initialLabel, ActionListener* firstListener, float _fontSize)
  : mHeld(false), mTimeSincePress(0), mResidualTime(0)
{
  setLabel(initialLabel);
  setFontSize(_fontSize);
  
  setEnabled(true);
  inside = false;

  addActionListener(firstListener);
}


Button::~Button()
{
}


void
Button::addActionListener(ActionListener* newListener)
{
  if(newListener != NULL)
    actionListeners.push_back(newListener);
}


// -------------------------------------------------------------------------------- 


bool
Button::isEnabled() const
{
  return enabled;
}

void
Button::setEnabled(bool isEnabled)
{
  enabled = isEnabled;
  markDirty();
}


// -------------------------------------------------------------------------------- 

void
Button::emitClick()
{
  if(!enabled) return;

  // REFACTOR: Add
//   SoundManager::playSoundEffect("guilib/button_click");  

  for(vector<ActionListener*>::const_iterator listenerI = actionListeners.begin();
      listenerI != actionListeners.end(); ++listenerI) {
    ActionListener* currentListener = *listenerI;
    currentListener->actionPerformed(this);
  }

//   for(vector<ButtonListener*>::const_iterator listenerI = buttonListeners.begin();
//       listenerI != buttonListeners.end(); ++listenerI) {
//     ButtonListener* currentListener = *listenerI;
//     currentListener->buttonClicked(this, clickCount);
//   }  
}

//--------------------------------------------------------------------------------

bool
Button::mouseDownEvent(int button, int, int)
{
  if(button != 0) return false;
  mHeld = true;
  mTimeSincePress = 0;
  mResidualTime = 0;
  emitClick();
  return true;
}

bool
Button::mouseUpEvent(int button, int, int)
{
  if(button != 0) return false;
  mHeld = false;
  return true;
}



// bool
// Button::mouseClickEvent(int button, unsigned, int , int )
// {
//   if(button == 0) {
//     return true;
//   }
//   else {
//     return false;
//   }  
// }




void
Button::mouseEnterEvent()
{
  // REFACTOR
//   SoundManager::playSoundEffect("guilib/button_rollover");  
  inside = true;
  markDirty();
}


void
Button::mouseLeaveEvent()
{
  inside = false;
  mHeld = false;
  markDirty();
}
 

//--------------------------------------------------------------------------------

const float MAGIC_BUTTON_REPEAT_DELAY = 1;
const float MAGIC_BUTTON_REPEAT_INTERVAL = 0.1;

void
Button::update(float timeStep)
{
  if(!mHeld) return;
  mTimeSincePress += timeStep;
  if(mTimeSincePress > MAGIC_BUTTON_REPEAT_DELAY) {
    mResidualTime += timeStep;
    while(mResidualTime > MAGIC_BUTTON_REPEAT_INTERVAL) {
      mResidualTime -= MAGIC_BUTTON_REPEAT_INTERVAL;
      emitClick();
    }
  }
}


//--------------------------------------------------------------------------------

void
Button::paint(Graphics* gfxContext)
{
  if(enabled) {
    if(inside) {
      gfxContext->setColor(GUIManager::getSkinColor(GUIManager::SKIN_BUTTON_MOUSEOVER_COLOR));
    }
    else {
      gfxContext->setColor(GUIManager::getSkinColor(GUIManager::SKIN_BUTTON_ENABLED_COLOR));
    }    
  }
  else {
    gfxContext->setColor(GUIManager::getSkinColor(GUIManager::SKIN_BUTTON_DISABLED_COLOR));
  }
    
  GRect buttonBounds;
  buttonBounds.setSize(getSize());
  gfxContext->fillRoundedRect(buttonBounds.x, buttonBounds.y, buttonBounds.w, buttonBounds.h);
  gfxContext->setColor(GUIManager::getSkinColor(GUIManager::SKIN_BUTTON_OUTLINE_COLOR));
  gfxContext->drawRoundedRect(buttonBounds.x, buttonBounds.y, buttonBounds.w, buttonBounds.h);  

  Label::paint(gfxContext);
}



//================================================================================
// Unit test

TEST_FIXTURE_DEFINITION("unit/guilib/Button", Test_Button);

namespace
{
  
  class TestActionListener : public ActionListener
  {
  public:
    TestActionListener()
    {
      receivedEvent = false;
    }
    
    virtual void actionPerformed(Widget*)
    {
      receivedEvent = true;
    }

    bool receivedEvent;
  };
}



void
Test_Button::event_emission()
{
  const wstring testLabel = L"testLabel";
  const float testFontSize = 12;

  Button* testButton;
  TestActionListener* testActionListener = new TestActionListener();

  testButton = new Button(testLabel);
  CHECK("Labels are equal after set", testButton->getLabel() == testLabel);

  testButton = new Button(testLabel, testActionListener);
  CHECK("Labels are equal after set", testButton->getLabel() == testLabel);

  testActionListener->receivedEvent = false;
  testButton->mouseClickEvent(0, 1, 0, 0);
  CHECK("Event was received", testActionListener->receivedEvent == true);
  delete testButton;

  testButton = new Button(testLabel, testActionListener, testFontSize);
  CHECK("Label is equal after set", testButton->getLabel() == testLabel);

  testActionListener->receivedEvent = false;
  testButton->mouseClickEvent(0, 1, 0, 0);
  CHECK("Event was received", testActionListener->receivedEvent == true);
  delete testButton;
}



