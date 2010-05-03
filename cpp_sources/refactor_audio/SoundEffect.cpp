//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "SoundEffect.hpp"
#include "SoundManager.hpp"
#ifndef SELF_TEST
#include "screens/LoadingScreen.hpp"
#endif


//--------------------------------------------------------------------------------
// Initializing the main screen

SoundEffect::SoundEffect(string _resourceName)
  : sample(NULL), sampleBuffer(NULL), sampleSize(0), resourceName(_resourceName)
{
#ifndef SELF_TEST
  LoadingScreen::tick(stringToWString(resourceName));
#endif

  sampleBuffer = loadResourceToMemory(resourceName, RESOURCE_GROUP_MAIN, "SoundEffect::SoundEffect()", &sampleSize);

  sample = FSOUND_Sample_Load(FSOUND_FREE, static_cast<const char*>(sampleBuffer), FSOUND_LOADMEMORY | FSOUND_LOOP_OFF | FSOUND_2D, 0, sampleSize);
  if(sample == NULL) {
    reportFMODError(string("FSOUND_Sample_Load: ") + resourceName);
    free(sampleBuffer);
    sampleBuffer = NULL;
  }
}


SoundEffect::~SoundEffect()
{
  if(sample != NULL)
    FSOUND_Sample_Free(sample);
  if(sampleBuffer != NULL)
    free(sampleBuffer);
}


//--------------------------------------------------------------------------------

void
SoundEffect::play()
{
  
  if(sample != NULL) {
    int channel = FSOUND_PlaySound(FSOUND_FREE, sample);
    if(channel == -1) {
      reportFMODError(string("FSOUND_PlaySound: ") + resourceName);
    }    
  }
}
