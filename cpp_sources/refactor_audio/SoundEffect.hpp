//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef AUDIO_SOUND_EFFECT_HPP
#define AUDIO_SOUND_EFFECT_HPP

class SoundEffect
{
public:
  SoundEffect(string resourceName);
  ~SoundEffect();

  void play();
private:
  string resourceName;
  
  size_t sampleSize;
  void* sampleBuffer;
  FSOUND_SAMPLE* sample;
};


#endif
