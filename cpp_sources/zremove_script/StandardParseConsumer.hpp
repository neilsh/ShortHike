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

#ifndef SCRIPT_STANDARD_PARSE_CONSUMER_HPP
#define SCRIPT_STANDARD_PARSE_CONSUMER_HPP

#include "Parser.hpp"

class StandardParseConsumer : public ParseConsumer
{
public:
  virtual void consumeSoundEffect(string soundName, SoundEffect* );
  virtual void consumeTrack(Track* newTrack);
  virtual void consumeAmbient(string ambientName, Ambient* newAmbient);
  virtual void consumeModule(string identifier, Module* newModule);
  virtual void consumePort(string identifier, Port* newPort);
};



#endif
