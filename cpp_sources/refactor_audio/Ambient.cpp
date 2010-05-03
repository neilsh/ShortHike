//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"


#include "Ambient.hpp"
#include "SoundManager.hpp"
#ifndef SELF_TEST
#include "screens/LoadingScreen.hpp"
#endif

//--------------------------------------------------------------------------------
// Initializing the main screen

Ambient::Ambient(string resourceName, string resourceGroup, float startVolume)
  : isActive(false), targetVolume(startVolume), currentVolume(startVolume),
    averageRunningTime(0), runningTime(0),
    base(NULL), baseBuffer(NULL), baseSize(0), baseChannel(-1)
{
#ifndef SELF_TEST
  LoadingScreen::tick(stringToWString(resourceName));
#endif

  baseBuffer = loadResourceToMemory(resourceName, resourceGroup, "Ambient::Ambient()", &baseSize);
}

Ambient::~Ambient()
{
  if(baseBuffer != NULL)
    free(baseBuffer);
}

//--------------------------------------------------------------------------------

void
Ambient::enterState()
{
  if(baseBuffer != NULL && baseSize > 0) {
    base = FSOUND_Stream_Open(static_cast<const char*>(baseBuffer), FSOUND_LOADMEMORY | FSOUND_LOOP_NORMAL, 0, baseSize);
    if(base == NULL) {
      reportFMODError("FSOUND_Stream_Open");
    }
    else {
      baseChannel = FSOUND_Stream_Play(FSOUND_FREE, base);
      if(baseChannel == -1) {
        reportFMODError("FSOUND_Stream_Play");
      }
      else {      
        isActive = true;
        play();
      }
    }
  }  
}

void
Ambient::exitState()
{
  
  isActive = false;
  targetVolume = 0;
}


//--------------------------------------------------------------------------------

void
Ambient::play()
{
  if(baseChannel != -1) {
    int channelVolume = clamp(static_cast<int>(currentVolume * 255), 0, 255);
    checkFMODError(FSOUND_SetVolume(baseChannel, channelVolume));
    targetVolume = 1;
    
    if(averageRunningTime > 0) {
      runningTime = averageRunningTime + (fnrand() * averageRunningTime * 0.25);
    }
    else {
      runningTime = 60;
    }
  }
}


bool
Ambient::isPlaying()
{
  return runningTime > 0;
}




//--------------------------------------------------------------------------------

void
Ambient::setRunningTime(float seconds)
{
  averageRunningTime = seconds;
}


//--------------------------------------------------------------------------------

const float MAGIC_FADE_TIME_SECONDS = 5.0;
const float MAGIC_AMBIENT_VOLUME = 0.75;


bool
Ambient::update(Time deltaT)
{
  if(baseBuffer == NULL || base == NULL || baseChannel == -1) return false;

  if(isActive == false && currentVolume == 0 && targetVolume == 0) {
    checkFMODError(FSOUND_Stream_Stop(base));
    checkFMODError(FSOUND_Stream_Close(base));
    return false;
  }
  
  if(currentVolume != targetVolume) {
    float fadeStep = deltaT * 1.0 / MAGIC_FADE_TIME_SECONDS;
    if(fadeStep > abs(targetVolume - currentVolume))
      currentVolume = targetVolume;
    else if(currentVolume > targetVolume)
      currentVolume -= fadeStep;
    else
      currentVolume += fadeStep;
  }

  int channelVolume = clamp(static_cast<int>(currentVolume * MAGIC_AMBIENT_VOLUME * 255), 0, 255);
  checkFMODError(FSOUND_SetVolume(baseChannel, channelVolume));
                   
  runningTime -= deltaT;
  if(runningTime <= 0) {
    targetVolume = 0;
    runningTime = 0;
  }

  return true;
}
