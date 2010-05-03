//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Persistent.hpp"
#include "ChunkData.hpp"

bool
Persistent::loadAll(ChunkData* chunkData)
{
  ChunkIterator cIterator = chunkData->begin();
  return load(cIterator, chunkData->end(), cIterator);
}  
