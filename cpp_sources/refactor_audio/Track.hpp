//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef AUDIO_TRACK_HPP
#define AUDIO_TRACK_HPP

#include "SoundUpdate.hpp"

class Track
{
public:
  Track(string resourceName, string resourceGroup = RESOURCE_GROUP_MAIN);
  ~Track();

  void play();
  void stop();
  bool isPlaying();
private:
  string resourceName;
  
  void* trackBuffer;
  size_t trackSize;
  FSOUND_STREAM* track;
  
  float volume;
  int trackLength;
};


#endif
