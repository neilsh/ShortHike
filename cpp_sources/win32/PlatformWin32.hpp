//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef MAIN_WIN32_PLATFORM_HPP
#define MAIN_WIN32_PLATFORM_HPP

#include "main/Platform.hpp"

class RenderSystem;
class Timer;


class PlatformWin32 : public Platform
{
public:
  virtual void pumpMessages();
  
  virtual RenderSystem* createRenderSystem();
  virtual Timer* createTimer();
  virtual void createProcess(string programName, string pArguments);

  virtual void openBuyPage();

  virtual bool openFileDialog(string& fileName);
  virtual bool saveFileDialog(string& fileName);

  virtual vector<FileEntry> scanAllFiles();
  virtual bool scanFile(string fileName, FileEntry& oFileEntry);

  virtual void* aquireFileMapping(string fileName, size_t& oFileSize);
  virtual void releaseFileMapping(string fileName);
private:
  struct FileMapping
  {
    int referenceCount;
    HANDLE mappingHandle;
    HANDLE fileHandle;
    size_t fileSize;
    void* rawData;
  };
  
  map<string, FileMapping> mOpenFileMappings;
};

#endif
