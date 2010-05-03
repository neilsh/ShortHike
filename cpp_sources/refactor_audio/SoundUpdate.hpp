//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef AUDIO_SOUND_UPDATE_HPP
#define AUDIO_SOUND_UPDATE_HPP

class SoundUpdate
{
public:
  virtual bool update(Time deltaT) = 0;
};


#endif
