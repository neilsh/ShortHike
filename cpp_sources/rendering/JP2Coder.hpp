//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#pragma once
#ifndef RENDERING_JP2CODER_HPP
#define RENDERING_JP2CODER_HPP

class ChunkStream;
class ChunkIterator;

extern bool compressJP2(void* data, int width, int height, ChunkStream* chunkStream);
extern bool decompressJP2(ChunkIterator iChunk, void*& data, int& oWidth, int& oHeight);

#endif
