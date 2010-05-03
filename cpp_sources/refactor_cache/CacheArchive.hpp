//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef CACHE_CACHE_ARCHIVE_HPP
#define CACHE_CACHE_ARVHIVE_HPP


class CacheArchive : public Archive
{
public:
  CacheArchive(const string& fileName);
  virtual ~CacheArchive ();
 
  const String& getName(void) const;
  virtual bool isCaseSensitive(void) const;
  virtual void load();
  virtual void unload();
  virtual DataStreamPtr open(const String &filename) const;
  virtual StringVectorPtr list(bool recursive = true);
  virtual FileInfoListPtr listFileInfo(bool recursive = true);
  virtual StringVectorPtr find(const String &pattern, bool recursive = true);
  virtual bool exists(const String &filename);
  virtual FileInfoListPtr findFileInfo(const String &pattern, bool recursive = true);
  const String& getType(void) const;

private:
  string fileName;
  string type;
  Archive* fileArchive;
};



#endif
