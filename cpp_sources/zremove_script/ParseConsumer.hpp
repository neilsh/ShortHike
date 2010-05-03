//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
//
//
//--------------------------------------------------------------------------------

#ifndef SCRIPT_PARSE_CONSUMER_HPP
#define SCRIPT_PARSE_CONSUMER_HPP

class ParseConsumer
{
public:
  virtual void consumeSoundEffect(SoundEffect* ) {}
  virtual void consumeTrack(Track* newTrack) {}
  virtual void consumeAmbient(Ambient* newAmbient) {}
  virtual void consumeModule(Module* newModule) {}
}


#endif
