//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------



#include "Common.hpp"

#include "StandardParseConsumer.hpp"
#include "audio/SoundManager.hpp"
#include "property/PrototypeManager.hpp"
#include "game/Module.hpp"
#include "game/Port.hpp"

//--------------------------------------------------------------------------------

void
StandardParseConsumer::consumeSoundEffect(string soundName, SoundEffect* newEffect)
{
  SoundManager::getSingleton().addSoundEffect(soundName, newEffect);
}


void
StandardParseConsumer::consumeTrack(Track* newTrack)
{
  SoundManager::getSingleton().addTrack(newTrack);
}


void
StandardParseConsumer::consumeAmbient(string ambientName, Ambient* newAmbient)
{
  SoundManager::getSingleton().addAmbient(ambientName, newAmbient);
}

void
StandardParseConsumer::consumeModule(string identifier, Module* prototype)
{
  PrototypeManager::getSingleton().addPrototype(identifier, prototype);
}

void
StandardParseConsumer::consumePort(string identifier, Port* port)
{
  PrototypeManager::getSingleton().addPrototype(identifier, port);
}
