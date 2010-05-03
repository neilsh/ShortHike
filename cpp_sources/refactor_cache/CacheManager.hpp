//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef CACHE_CACHE_MANAGER_HPP
#define CACHE_CACHE_MANAGER_HPP

class CacheImageCodec;
class CompressedImageCodec;

class CacheManager : public Singleton<CacheManager>, public ArchiveFactory
{
public:
  CacheManager();
  ~CacheManager();
  
  virtual const String& getType() const;
  virtual Archive* createInstance(const String &name);
  virtual void destroyInstance(Archive *);
private:
  CacheImageCodec* cacheImageCodec;
  CompressedImageCodec* compressedImageCodec;

  string type;
};

#endif
