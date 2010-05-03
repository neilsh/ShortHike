//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#pragma once
#ifndef MAIN_PLATFORM_HPP
#define MAIN_PLATFORM_HPP

class RenderSystem;
class Timer;

struct FileEntry
{
  string fileName;
  size_t fileSize;
  long timeStampLow;
  long timeStampHigh;
};


class Platform
{
public:
  virtual void pumpMessages() = 0;

  virtual RenderSystem* createRenderSystem() = 0;
  virtual Timer* createTimer() = 0;
  virtual void createProcess(string programName, string pArguments) = 0;

  virtual void openBuyPage() = 0;

  virtual bool openFileDialog(string& fileName) = 0;
  virtual bool saveFileDialog(string& fileName) = 0;

  virtual vector<FileEntry> scanAllFiles() = 0;
  virtual bool scanFile(string fileName, FileEntry& oFileEntry) = 0;
  
  virtual void* aquireFileMapping(string fileName, size_t& oFileSize) = 0;
  virtual void releaseFileMapping(string fileName) = 0;
};

#endif
