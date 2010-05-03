//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "LoadingScreen.hpp"

#include "main/Main.hpp"
#include "widgets/LoadingDialog.hpp"
#include "guilib/GUIManager.hpp"


//--------------------------------------------------------------------------------
// Singleton implementation

template<> LoadingScreen*
Singleton<LoadingScreen>::ms_Singleton = 0;


//--------------------------------------------------------------------------------

LoadingScreen::LoadingScreen(Main* _main)
  : main(_main), loadingTimer(NULL), isLoading(false)
{
  Root& ogreRoot = Root::getSingleton();
  sceneManager = new SceneManager();
  sceneManager->_setDestinationRenderSystem(ogreRoot.getRenderSystem());
  camera = sceneManager->createCamera("LoadingScreen camera");

  loadingDialog = new LoadingDialog("LoadingScreen");
  loadingTimer = PlatformManager::getSingleton().createTimer();

  ResourceGroupManager::getSingleton().addResourceGroupListener(this);
}


//--------------------------------------------------------------------------------

void
LoadingScreen::enterState()
{
  Screen::enterState();
  
//   Viewport* viewport = main->getRenderWindow()->addViewport(camera);
//   viewport->setBackgroundColour(ColourValue(0,0,0));
  
//   rGUIManager()->setRootListener(NULL);
//   rGUIManager()->pushDialog(loadingDialog);
}


void
LoadingScreen::exitState()
{
//   rGUIManager()->popDialog();

//   rGUIManager()->setRootListener(NULL);  
//   main->getRenderWindow()->removeAllViewports();
  
  Screen::exitState();
}


//--------------------------------------------------------------------------------

void
LoadingScreen::startTask(wstring taskName, int itemNumber)
{
  loadingDialog->startTask(taskName);
  for(float tick = 0; tick <= itemNumber; ++tick){
    storedTickPoints.push_back(tick / itemNumber);
  }
  currentTick = 0;
  tickPoints.clear();
  loadingTimer->reset();
  isLoading = true;
}

void
LoadingScreen::startTask(wstring taskName, vector<Real> _tickPoints)
{
  loadingDialog->startTask(taskName);
  logEngineInfo("LoadingScreen::starTask: Assigning tickpoints");
  storedTickPoints = _tickPoints;
  currentTick = 0;
  tickPoints.clear();
  logEngineInfo("LoadingScreen::starTask: Reseting timers");
  loadingTimer->reset();
  isLoading = true;
  logEngineInfo(string("LoadingScreen::startTask: ") + wstringToString(taskName));
}


void
LoadingScreen::tick(wstring itemName)
{
  if(getSingleton().isLoading == false) return;
  if(itemName != L"")
    logEngineInfo(string("Tick: ") + wstringToString(itemName));
  getSingleton().tickImpl(itemName);
}



void
LoadingScreen::tickImpl(wstring itemName)
{
  
  if(currentTick < storedTickPoints.size())
    loadingDialog->setTickItem(itemName, storedTickPoints[currentTick]);
  else
    loadingDialog->setTickItem(itemName, 1.0);
  currentTick++;
  tickPoints.push_back(loadingTimer->getMilliseconds());
}

void
LoadingScreen::endTask()
{
  loadingDialog->endTask();
  Real finalTime = static_cast<Real>(loadingTimer->getMilliseconds());
  for(vector<Real>::iterator tickI = tickPoints.begin(); tickI != tickPoints.end(); ++tickI) {
    *tickI /= finalTime;
  }
  ostringstream tickDataStream;
  tickDataStream << "LoadingScreen::endTask: Total number of ticks: " << tickPoints.size() << " loaded in " << (finalTime * 0.001) << "s";
  logEngineInfo(tickDataStream.str());
  isLoading = false;
}

//--------------------------------------------------------------------------------

vector<Real>
LoadingScreen::getNormalizedTicks()
{
  return tickPoints;
}


//--------------------------------------------------------------------------------

void
LoadingScreen::resourceGroupScriptingStarted (const String &, size_t scriptCount)
{
  tick();
}

void
LoadingScreen::scriptParseStarted (const String &scriptName)
{
  tick(stringToWString(scriptName));
}

void
LoadingScreen::scriptParseEnded ()
{
  tick();
}

void
LoadingScreen::resourceGroupScriptingEnded (const String &)
{
  tick();
}

void
LoadingScreen::resourceGroupLoadStarted (const String &, size_t resourceCount)
{
  tick();
}

void
LoadingScreen::resourceLoadStarted (const ResourcePtr &resource)
{
  tick(stringToWString(resource->getName()));
}

void
LoadingScreen::resourceLoadEnded (void)
{
  tick();
}

void
LoadingScreen::worldGeometryStageStarted (const String &)
{
  tick();
}

void
LoadingScreen::worldGeometryStageEnded (void)
{
  tick();
}

void
LoadingScreen::resourceGroupLoadEnded (const String &)
{
  tick();
}

