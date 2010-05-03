//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "ActivateDialog.hpp"
#include "config/ConfigManager.hpp"
#include "config/Config.hpp"
#include "drm/License.hpp"
#include "guilib/Button.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/Label.hpp"
#include "guilib/TextField.hpp"
#include "script/LuaManager.hpp"
#include "widgets/LoadingDialog.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> ActivateDialog*
Singleton<ActivateDialog>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

const int MAGIC_ACTIVATE_DIALOG_WIDTH = 540;
const int MAGIC_ACTIVATE_DIALOG_HEIGHT = 100;

ActivateDialog::ActivateDialog()
  : Dialog("ActivateDialog")
{
  setBackground(new FrameBackground());
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_ACTIVATE_DIALOG_WIDTH, MAGIC_ACTIVATE_DIALOG_HEIGHT);
  centerOnScreen();

  mInstructions = new Label(L"Please type in your ordering email address");
  mInstructions->setSize(480, 25);
  mInstructions->setPosition(10, 10);
  add(mInstructions);

  mEmailField = new TextField();
  mEmailField->setSize(480, 25);
  mEmailField->setPosition(10, 40);
  add(mEmailField);

  mActivateButton = new Button(L"Activate");
  mActivateButton->setSize(60, 20);
  mActivateButton->setPosition(370, 70);
  mActivateButton->addActionListener(this);
  add(mActivateButton);

  mCancelButton = new Button(L"Cancel");
  mCancelButton->setSize(60, 20);
  mCancelButton->setPosition(433, 70);
  mCancelButton->addActionListener(this);
  add(mCancelButton);  

#ifndef SELF_TEST
  mLoadingDialog = new LoadingDialog("ActivateDialog");
#endif
}

//--------------------------------------------------------------------------------

bool
ActivateDialog::hasActiveLicense(string scriptCommand)
{
  mScriptCommand = scriptCommand;
  License* currentLicense = ConfigManager::getCurrent()->getLicense();
  if(currentLicense->getType() == LICENSE_ACTIVE) return true;
  
#ifndef SELF_TEST
  mLoadingDialog->show();
  mLoadingDialog->startTask(L"Activating..", 0.0f);
  currentLicense->updateLicenseKey();
  mLoadingDialog->hide();
#endif

  if(currentLicense->getType() == LICENSE_TRIAL) {
    mInstructions->setLabel(L"Please type in your ordering email address");
    mEmailField->setLabel(L"");
//     rGUIManager()->pushDialog(this);
    return false;
  }
  else {
    return true;
  }
}

//--------------------------------------------------------------------------------

void
ActivateDialog::actionPerformed(Widget* source)
{
  if(source == mCancelButton) {
//     rGUIManager()->popDialog();
  }
  else if(source == mActivateButton) {
    License* currentLicense = ConfigManager::getCurrent()->getLicense();
    currentLicense->setPlayerEmail(wstringToString(mEmailField->getLabel()));

#ifndef SELF_TEST
    mLoadingDialog->show();
    mLoadingDialog->startTask(L"Activating..", 0.0f);
    currentLicense->updateLicenseKey();
    mLoadingDialog->hide();
#endif

    if(currentLicense->getType() == LICENSE_TRIAL) {
      mInstructions->setLabel(L"Email not found: Please enter valid ordering email");
      mEmailField->setLabel(L"");
    }
    else {
//       rGUIManager()->popDialog();
      if(mScriptCommand != "")
        LuaManager::getSingleton().doString(mScriptCommand);
    }    
  }
}
