//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "CacheManager.hpp"
#include "CacheImageCodec.hpp"
#include "CompressedImageCodec.hpp"
#include "CacheArchive.hpp"


//--------------------------------------------------------------------------------
// Singleton implementation

template<> CacheManager*
Singleton<CacheManager>::ms_Singleton = 0;


//--------------------------------------------------------------------------------
// Initializing the main screen

CacheManager::CacheManager()
  : type("Cache")
{
  cacheImageCodec = new CacheImageCodec();
  compressedImageCodec = new CompressedImageCodec();
  ImageCodec::registerCodec(cacheImageCodec);

  ArchiveManager::getSingleton().addArchiveFactory(this);
}


CacheManager::~CacheManager()
{
}


//--------------------------------------------------------------------------------
// Archive factory methods

 
const String&
CacheManager::getType() const
{
  return type;
}

Archive*
CacheManager::createInstance(const String &filePath)
{
  return new CacheArchive(filePath);
}

void
CacheManager::destroyInstance(Archive *archive)
{
  delete archive;
}
