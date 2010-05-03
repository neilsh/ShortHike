//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "LoadingDialog.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/Label.hpp"
#include "guilib/ProgressBar.hpp"
#include "guilib/RasterImage.hpp"
#include "main/Main.hpp"
#include "version.hpp"
#include "generated/build_info.hpp"
#include "drm/License.hpp"
#include "config/ConfigManager.hpp"
#include "config/Config.hpp"

//--------------------------------------------------------------------------------

const int MAGIC_LOADING_DIALOG_WIDTH = 500;
const int MAGIC_LOADING_DIALOG_HEIGHT = 150;

LoadingDialog::LoadingDialog(string namePrefix)
  : Dialog(namePrefix + "_LoadingDialog"), mByteProgress(false)
{
  setBackground(new FrameBackground());
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setSize(MAGIC_LOADING_DIALOG_WIDTH, MAGIC_LOADING_DIALOG_HEIGHT);
  centerOnScreen();

  frameDetail = new RasterImage("LoadingDialogDetail.png", 0, RESOURCE_GROUP_LOADING);
  frameDetail->setSize(500, 150);
  frameDetail->setPosition(0, 0);
  add(frameDetail);

  taskName = new Label(L"CurrentState");
  taskName->setSize(480, 50);
  taskName->setPosition(10, 5);
  taskName->setFontSize(25);
  add(taskName);

  itemName = new Label();
  itemName->setSize(480, 20);
  itemName->setPosition(10, 85);
  itemName->setFontSize(12);
  
  mNumericalProgress = new Label();
  mNumericalProgress->setHorizontalAlignment(Label::RIGHT);
  mNumericalProgress->setSize(480, 12);
  mNumericalProgress->setPosition(10, 96);
  mNumericalProgress->setFontSize(12);
  mNumericalProgress->setVisible(false);
  add(mNumericalProgress);
  
  // Make this visible when debugging ticks
//   if(ConfigManager::getCurrent()->getShowLoadingItems() == true)
  add(itemName);

  progressBar = new ProgressBar();
  progressBar->setSize(480, 20);
  progressBar->setPosition(10, 115);
  add(progressBar);
}

//--------------------------------------------------------------------------------

void
LoadingDialog::startTask(wstring _taskName, float maxTick)
{
  mMaxTick = maxTick;
  taskName->setLabel(_taskName);
  if(mMaxTick == 0) {
    progressBar->setVisible(false);
  }
  else {
    progressBar->setVisible(true);
    progressBar->setMax(mMaxTick);
    progressBar->setCurrent(0.0f);
  }  
  itemName->setLabel(L"");
  updateNumerical();
  rMain()->renderAndUpdate();  
}

void
LoadingDialog::setTickItem(wstring _itemName, float currentTick)
{
  progressBar->setCurrent(currentTick);
  if(_itemName != L"")
    itemName->setLabel(_itemName);
  updateNumerical();
  rMain()->renderAndUpdate();  
}

void
LoadingDialog::reportProgress(float progress)
{
  progressBar->setCurrent(progressBar->getCurrent() + progress);
  updateNumerical();
  rMain()->renderAndUpdate();
}


void
LoadingDialog::endTask()
{
  progressBar->setCurrent(mMaxTick);
  updateNumerical();
  rMain()->renderAndUpdate();  
}


//--------------------------------------------------------------------------------

void
LoadingDialog::setByteProgress(bool byteProgress)
{
  mByteProgress = byteProgress;
  mNumericalProgress->setVisible(byteProgress);
}

void
LoadingDialog::updateNumerical()
{
  wostringstream wossNum;
  wossNum << fixed << setprecision(1) << progressBar->getCurrent() * 0.000001f << " MB of " << setprecision(1) << mMaxTick * 0.000001f << " MB";
  mNumericalProgress->setLabel(wossNum.str());
}

