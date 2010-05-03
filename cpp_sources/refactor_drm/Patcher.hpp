//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef DRM_PATCH_MANAGER_HPP
#define DRM_PATCH_MANAGER_HPP

#include "Wininet.h"

class LoadingDialog;
class ProgressListener;

extern bool patcherUpdateAll();

struct MD5File
{
  string md5Sum;
  size_t fileSize;
  string fileName;
};

inline bool operator<(const MD5File& a, const MD5File& b)
{
  if(a.md5Sum != b.md5Sum)
    return a.md5Sum < b.md5Sum;
  if(a.fileName != b.fileName)
    return a.fileName < b.fileName;
  return a.fileSize < b.fileSize;
}

inline bool operator==(const MD5File& a, const MD5File& b)
{
  return a.md5Sum == b.md5Sum && a.fileName == b.fileName && a.fileSize == b.fileSize;
}

inline bool lessMD5Name(MD5File& a, MD5File& b)
{
  return a.fileName < b.fileName;
}



class Patcher
{
public:
  Patcher(string localPath, string remotepath, bool deleteEmptyDirectories = true);

  bool scan(vector<MD5File>* downloadFiles = NULL, ProgressListener* loadingDialog = NULL, bool forceNetwork = false);
  void update(ProgressListener* loadingDialog = NULL, bool forceNetwork = false);
private:
  // Core patch flow
  bool getServerTextFile(string fileURL, string& textFile);
  vector<MD5File> parseServerFileList(string fileListData);
  vector<MD5File> buildLocalFileList();
  void buildFileSets(vector<MD5File> localFiles, vector<MD5File> serverFiles);
  void deleteLocalFiles();
  bool downloadServerFiles();
  void deleteEmptyDirectories();

  // Util methods
  bool isDirectoryEmpty(string directoryPath, vector<string>& emptyDirectories);

            
  string mLocalPath;
  string mRemotePath;
  bool mDeleteEmptyDirectories;
  vector<MD5File> mDownloadFiles;
  vector<MD5File> mDeleteFiles;

  HINTERNET mInternetBase;
  HINTERNET mServerConnection;

  ProgressListener* mProgressListener;
};

//--------------------------------------------------------------------------------

class TestPatcherUpdate : public TestFixture
{
public:
  TEST_FIXTURE_START(TestPatcherUpdate);
  TEST_CASE(update_test_directory);
  TEST_FIXTURE_END();

  void update_test_directory();
};


#endif
