//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#include "Common.hpp"

#include "FunctionExporters.hpp"
#include "audio/SoundManager.hpp"
#include "config/ConfigManager.hpp"
#include "config/Config.hpp"
#include "drm/ActivateDialog.hpp"
#include "drm/License.hpp"
#include "drm/Patcher.hpp"
#include "main/Main.hpp"
#include "widgets/LoadingDialog.hpp"

//--------------------------------------------------------------------------------

int
drm_activate(LuaState* state)
{
  ActivateDialog* activateDialog = ActivateDialog::getSingletonPtr();
  activateDialog->hasActiveLicense();
  return 0;
}


//--------------------------------------------------------------------------------

int
drm_update(LuaState* )
{
  Config* currentConfig = ConfigManager::getCurrent();
  License* license = currentConfig->getLicense();
  license->updateLicenseKey();
  // FIXME: Maybe ask the user if they want to upgrade?
  if(license->hasExpired()) {
    SoundManager::playSoundEffect("guilib/escape");
    return 0;
  }
  
  ActivateDialog* activateDialog = ActivateDialog::getSingletonPtr();
  if(activateDialog->hasActiveLicense("drm_update()") == false) return 0;

  if(false == patcherUpdateAll()) return 0;

  rMain()->quitApplication();
  return 0;
}

//--------------------------------------------------------------------------------

int
drm_toggle_distribution(LuaState*)
{
  Config* currentConfig = ConfigManager::getCurrent();
  currentConfig->setCompleteDistribution(!currentConfig->isCompleteDistribution());
  ConfigManager::getSingleton().saveConfig();
  return 0;
}

//--------------------------------------------------------------------------------

int
drm_buy(LuaState* state)
{
  openBuyPage();
  return 0;
}

//--------------------------------------------------------------------------------

void
registerDRMFunctions(LuaState* state)
{
  LuaObject globals = state->GetGlobals();

  globals.Register("drm_activate", &drm_activate);
  globals.Register("drm_update", &drm_update);
  globals.Register("drm_toggle_distribution", &drm_toggle_distribution);
  globals.Register("drm_buy", &drm_buy);
}
