//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef AUDIO_AMBIENT_HPP
#define AUDIO_AMBIENT_HPP

#include "SoundUpdate.hpp"

class Ambient : public State, public SoundUpdate
{
public:
  Ambient(string resourceName, string resourceGroup = RESOURCE_GROUP_MAIN, float startVolume = 0.0);
  ~Ambient();

  virtual void enterState();
  virtual void exitState();

  void play();
  bool isPlaying();

  void setRunningTime(float averageRunningTime);

  virtual bool update(Time deltaT);
private:
  bool isActive;
  float currentVolume;
  float targetVolume;

  void* baseBuffer;
  size_t baseSize;
  FSOUND_STREAM* base;
  int baseChannel;

  float averageRunningTime;
  float runningTime;
};


#endif
