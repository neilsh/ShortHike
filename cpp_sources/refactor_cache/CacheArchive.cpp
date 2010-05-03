//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "CacheArchive.hpp"


//--------------------------------------------------------------------------------

CacheArchive::CacheArchive(const string& fileName)
  : Archive(fileName, "Cache")
{
  fileArchive = ArchiveManager::getSingleton().load(fileName, "FileSystem");
  type = "Cache";
}

CacheArchive::~CacheArchive()
{
  delete fileArchive;
}


const String&
CacheArchive::getType(void) const
{
  return type;
}

//--------------------------------------------------------------------------------

const String&
CacheArchive::getName(void) const
{
  return fileName;
}

bool
CacheArchive::isCaseSensitive(void) const
{
  return fileArchive->isCaseSensitive();
}

void
CacheArchive::load()
{
  fileArchive->load();
}

void
CacheArchive::unload()
{
  fileArchive->unload();
}

//--------------------------------------------------------------------------------

// CALLED BY OGRE LOADER
DataStreamPtr
CacheArchive::open(const String &fileName) const
{
  string baseName = fileName.substr(0, fileName.size() - 3);
  string suffix = fileName.substr(fileName.size() - 3, 3);
  // Return an empty DataStream that is captured by our image coded. It only acts as a handle
  if(suffix == "shi") {
    return SharedPtr<DataStream>(new MemoryDataStream(fileName, (void *)NULL, 0));
  }
  else {
    return fileArchive->open(fileName);
  }  
}


// CALLED BY OGRE LOADER
StringVectorPtr
CacheArchive::find(const String &pattern, bool recursive)
{
  return fileArchive->find(pattern, recursive);
}


// CALLED BY OGRE LOADER
bool
CacheArchive::exists(const String &fileName)
{
  string baseName = fileName.substr(0, fileName.size() - 3);
  string suffix = fileName.substr(fileName.size() - 3, 3);
  if(suffix == "shi") {
    return fileArchive->exists(baseName + "shc") || fileArchive->exists(baseName + "tga") || fileArchive->exists(baseName + "jpg");
  }
  else {
    return fileArchive->exists(fileName);
  }  
}

//--------------------------------------------------------------------------------

StringVectorPtr
CacheArchive::list(bool recursive)
{
  return fileArchive->list(recursive);  
}


FileInfoListPtr
CacheArchive::listFileInfo(bool recursive)
{
  return fileArchive->listFileInfo(recursive);
}


FileInfoListPtr
CacheArchive::findFileInfo(const String &pattern, bool recursive)
{
  return fileArchive->findFileInfo(pattern, recursive);
}


