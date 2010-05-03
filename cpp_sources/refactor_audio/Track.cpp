//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "Track.hpp"
#include "SoundManager.hpp"

#ifndef SELF_TEST
#include "screens/LoadingScreen.hpp"
#endif


//--------------------------------------------------------------------------------
// Initializing the main screen

const float MAGIC_MUSIC_VOLUME = 0.55;

Track::Track(string _resourceName, string resourceGroup)
  : volume(MAGIC_MUSIC_VOLUME), resourceName(_resourceName)
{
#ifndef SELF_TEST
  LoadingScreen::tick(stringToWString(resourceName));
#endif

  trackBuffer = loadResourceToMemory(resourceName, resourceGroup, "Track::Track()", &trackSize);
}

Track::~Track()
{
  if(trackBuffer != NULL)
    free(trackBuffer);
}

//--------------------------------------------------------------------------------

void
Track::play()
{
  track = FSOUND_Stream_Open(static_cast<const char*>(trackBuffer), FSOUND_LOADMEMORY | FSOUND_LOOP_OFF, 0, trackSize);
  if(track == NULL) {
    reportFMODError(string("FSOUND_Stream_Open: ") + resourceName);
  }
  else {
    int trackChannel = FSOUND_Stream_Play(FSOUND_FREE, track);
    if(trackChannel == -1) {
      reportFMODError(string("FSOUND_Stream_Play: ") + resourceName);
    }
    else {
      trackLength = FSOUND_Stream_GetLengthMs(track);
      if(trackLength == 0) {
        reportFMODError(string("FSOUND_Stream_GetLengthMs: ") + resourceName);
      }
      else {
        int channelVolume = clamp(static_cast<int>(volume * 255.0), 0, 255);
        checkFMODError(FSOUND_SetVolume(trackChannel, channelVolume));
      }
    }
  }
  
}

void
Track::stop()
{
  if(track != NULL) {
    checkFMODError(FSOUND_Stream_Stop(track));
    checkFMODError(FSOUND_Stream_Close(track));
  }  
}


//--------------------------------------------------------------------------------

bool
Track::isPlaying()
{
  int streamPos = FSOUND_Stream_GetTime(track);
  if(streamPos == 0) {
    reportFMODError(string("FSOUND_Stream_GetTime: " + resourceName));
  }
  return streamPos < trackLength;
}
