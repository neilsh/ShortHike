//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
//
// Semantics:
// - Files are only checked when first aquired. If you desire a recheck then release
// the file and reaquire it.
// - The cache is rebuilt if coverage goes under a certain threshold, it can also be
// forced (like after an update). All files need to be released for this to work.

#pragma once
#ifndef FILE_CACHE_MANAGER_HPP
#define FILE_CACHE_MANAGER_HPP

// Generic class for all ShortHike file read access

class ChunkData;
struct FileEntry;

struct CacheFile
{
  string fileName;
  size_t fileSize;
  long timeStampLow;
  long timeStampHigh;

  bool inCache;
  int referenceCount;
  ChunkData* chunkData;
  void* data;
};


class CacheManager
{
public:
  CacheManager();

  bool lazyRebuild();
  
  bool aquireChunkData(string fileName, ChunkData*& oChunkData);
  bool releaseChunkData(string fileName);
  
  bool hasFileChanged(string fileName);

  const map<string, CacheFile>& getAllFiles();
private:
  bool isCacheAquired();
  bool aquireCache();
  bool releaseCache();
  bool rebuildCache();

  bool scanHeader(CacheFile& cacheFile);
  bool verifyFile(const FileEntry& fileEntry);
  bool isFileStale(const CacheFile& cacheFile);
  bool isFileStale(const CacheFile& cacheFile, const FileEntry& fileEntry);
  
  ChunkData* mCacheInfo;
  size_t mCacheSize;
  void* mCacheData;
  float mCacheCoverage;

  typedef map<string, CacheFile>::iterator CacheFileIterator;
  map<string, CacheFile> mCacheFiles;
};



#endif
