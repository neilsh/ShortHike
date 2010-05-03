//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"
#include "LimitDialog.hpp"
#include "version.hpp"

#include "ActivateDialog.hpp"
#include "config/ConfigManager.hpp"
#include "config/Config.hpp"
#include "drm/License.hpp"
#include "guilib/Button.hpp"
#include "guilib/FlowLayout.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/RasterImage.hpp"
#include "guilib/TextField.hpp"
#include "main/Main.hpp"
#include "script/LuaManager.hpp"
#include "widgets/LoadingDialog.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> LimitDialog*
Singleton<LimitDialog>::ms_Singleton = 0;


//--------------------------------------------------------------------------------

bool
showLimitDialog(bool quitMode)
{
  License* currentLicense = ConfigManager::getCurrent()->getLicense();
  if(currentLicense->isActive()) return false;

  if(false == LimitDialog::getSingleton().getVisible()) {
    LimitDialog::getSingleton().show();
    LimitDialog::getSingleton().setQuitMode(quitMode);
  }
  return true;
}

//--------------------------------------------------------------------------------

const int MAGIC_LIMIT_DIALOG_WIDTH = 450;
const int MAGIC_LIMIT_DIALOG_HEIGHT = 400;

LimitDialog::LimitDialog()
  : Dialog("LimitDialog"), mQuitMode(true)
{
  setBackground(new FrameBackground(true));
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_LIMIT_DIALOG_WIDTH, MAGIC_LIMIT_DIALOG_HEIGHT);
  centerOnScreen();

  GridLayout* dialogLayout = new GridLayout(this, 1, 8);
  dialogLayout->set_borders(4);
  dialogLayout->set_padding(3);

  mBuyMessage = new RasterImage("user_interface/texture/BuyMessage.png", 0);
  mBuyMessage->setSize(400, 300);
  mBuyMessage->setPosition(25, 25);
  add(mBuyMessage);
  
  mBuyButton = new Button(stringToWString(BUILD_BUY_NOW), this);
  mBuyButton->setFontSize(18);
  mActivateButton = new Button(L"Activate Full License", this);
  mActivateButton->setFontSize(18);
  mQuitButton = new Button(L"Quit", this);
  mQuitButton->setFontSize(18);

  Widget* buttonArea = new Widget(new FlowLayout(FlowLayout::HORIZONTAL, FlowLayout::RIGHT));
  buttonArea->add(mBuyButton);
  buttonArea->add(mActivateButton);
  buttonArea->add(mQuitButton);
  dialogLayout->add(buttonArea, 0, 7);

  layout();
}

//--------------------------------------------------------------------------------

void
LimitDialog::setQuitMode(bool quitMode)
{
  mQuitMode = quitMode;
  if(mQuitMode) {
    mQuitButton->setLabel(L"Quit");
  }
  else {
    mQuitButton->setLabel(L"Later");
  }
  layout();
}




//--------------------------------------------------------------------------------

void
LimitDialog::update(float timeStep)
{
  License* currentLicense = ConfigManager::getCurrent()->getLicense();
  if(currentLicense->isActive()) {
    setVisible(false);
  }  
}


//--------------------------------------------------------------------------------

void
LimitDialog::actionPerformed(Widget* source)
{
  if(source == mQuitButton) {
#ifndef SELF_TEST
    if(mQuitMode)
      rMain()->quitApplication();
    else
      setVisible(false);
#endif
  }
  else if(source == mBuyButton) {
    openBuyPage();
  }
  else if(source == mActivateButton) {
    if(true == ActivateDialog::getSingleton().hasActiveLicense()) {
      setVisible(false);
    }
  }
}
