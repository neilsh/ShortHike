//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "Patcher.hpp"
#include "License.hpp"
#include "md5.hpp"
#include "version.hpp"
#include "guilib/ProgressListener.hpp"
#include "config/ConfigManager.hpp"
#include "config/Config.hpp"
#include "widgets/LoadingDialog.hpp"

//--------------------------------------------------------------------------------

bool
patcherUpdateAll()
{
  static LoadingDialog* downloadDialog = new LoadingDialog("patcherUpdateAll:DownloadDialog");

  string targetName;
#ifdef BUILD_UNSTABLE
  targetName = "trunk";
#else
  targetName = "stable";
#endif
#ifdef _DEBUG
  Patcher* assetsPatcher;
  if(ConfigManager::getCurrent()->isCompleteDistribution())
    assetsPatcher = new Patcher("testdata/TestFilePatcherUpdate/assets", targetName + "/assets_complete");
  else
    assetsPatcher = new Patcher("testdata/TestFilePatcherUpdate/assets", targetName + "/assets_standard");
  Patcher* binaryPatcher = new Patcher("testdata/TestFilePatcherUpdate/binary_win32", targetName + "/binary_win32");
#else
  Patcher* assetsPatcher;
  if(ConfigManager::getCurrent()->isCompleteDistribution())
    assetsPatcher = new Patcher("assets", targetName + "/assets_complete");
  else
    assetsPatcher = new Patcher("assets", targetName + "/assets_standard");
  Patcher* binaryPatcher = new Patcher("binary_win32", targetName + "/binary_win32");
#endif
  
  downloadDialog->show();
  downloadDialog->startTask(L"Scanning for updates..", 0);

  float totalDownload = 0;
  bool needsUpdate = false;

  logEngineInfo("Scanning assets..");
  vector<MD5File> assetsFileList;
  if(true == assetsPatcher->scan(&assetsFileList, downloadDialog)) needsUpdate = true;
  for(vector<MD5File>::const_iterator iFile = assetsFileList.begin(); iFile != assetsFileList.end(); ++iFile) {
    const MD5File& file = *iFile;
    totalDownload += file.fileSize;
  }

  logEngineInfo("Scanning binary..");
  vector<MD5File> binaryFileList;
  if(true == binaryPatcher->scan(&binaryFileList, downloadDialog)) needsUpdate = true;
  for(vector<MD5File>::const_iterator iFile = binaryFileList.begin(); iFile != binaryFileList.end(); ++iFile) {
    const MD5File& file = *iFile;
    totalDownload += file.fileSize;
  }
  if(false == needsUpdate) {
    downloadDialog->hide();
    ConfigManager::getCurrent()->setUpdateInProgress(false);
    ConfigManager::getSingleton().saveConfig();
    return false;
  }

  // Force the update status to true. This is used to recover after a failed update
  ConfigManager::getCurrent()->setUpdateInProgress(true);
  ConfigManager::getSingleton().saveConfig();
  
  logEngineInfo("Downloading new files..");
  downloadDialog->startTask(L"Downloading..", totalDownload);
  downloadDialog->setByteProgress(true);
  binaryPatcher->update(downloadDialog);
  assetsPatcher->update(downloadDialog);
  downloadDialog->setByteProgress(false);

  // Force the update status to false. After this normal startup is possible
  ConfigManager::getCurrent()->setUpdateInProgress(false);
  ConfigManager::getSingleton().saveConfig();

  downloadDialog->startTask(L"Restarting..", 0);
  ostringstream commandLine;
  commandLine << GetCurrentProcessId();
#ifdef _DEBUG
  SimpleCreateProcess("debug_HotCopy/HotCopy.exe", commandLine.str());
#else
  SimpleCreateProcess("binary_win32/HotCopy.exe", commandLine.str());
#endif
  delete assetsPatcher;
  delete binaryPatcher;

  return true;
}


//--------------------------------------------------------------------------------
// Initializing the main screen

const string MAGIC_UPDATE_SERVER = "update.shorthike.com";
const int MAGIC_BUFFER_SIZE = 8192;

Patcher::Patcher(string localPath, string remotePath, bool deleteEmpty)
  : mLocalPath(localPath), mRemotePath(remotePath), mDeleteEmptyDirectories(deleteEmpty),
    mServerConnection(NULL), mInternetBase(NULL)
{
}

//--------------------------------------------------------------------------------

bool
Patcher::scan(vector<MD5File>* downloadFiles, ProgressListener* progressListener, bool forceNetwork)
{
  mProgressListener = progressListener;
  mDownloadFiles.clear();
  mDeleteFiles.clear();

  if(forceNetwork == true)
    if(ERROR_SUCCESS != InternetAttemptConnect(0)) return false;

  mInternetBase = InternetOpen("ShortHike", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
  if(NULL == mInternetBase) return false;
  
#ifndef SELF_TEST
  License* currentLicense = ConfigManager::getCurrent()->getLicense();
  string licenseKey = currentLicense->getLicenseKey();
#else
  string licenseKey = "KNIK9N8R3TQDC8K2P59I6UEC0OOP5O6QN53A9QC2AGININFQ68JO5O7U46RBO32NOBUD7L323BR78F4SLHHHDBV11L977HS3BDJPOUBO5NNS38VH996GG22D7KCL406HBJ7RJLLMBJN9QEJ67CA38LVGCPRMI2H57FNRTTOFS58CBJHS1CD5AJ9M33TVQ8RRBK37RQER3FNN";
#endif

  mServerConnection = InternetConnect(mInternetBase, MAGIC_UPDATE_SERVER.c_str(), INTERNET_DEFAULT_HTTP_PORT,
                                      licenseKey.c_str(), licenseKey.c_str(),
                                      INTERNET_SERVICE_HTTP, NULL, NULL);  
  if(mServerConnection == NULL) {
    InternetCloseHandle(mInternetBase);
    mInternetBase = NULL;
    return false;
  }
  
  string fileListData;
  if(false == getServerTextFile(mRemotePath + "/filelist.txt", fileListData)) {
    InternetCloseHandle(mServerConnection);
    mServerConnection = NULL;
    InternetCloseHandle(mInternetBase);
    mInternetBase = NULL;
    return false;
  }
  
  vector<MD5File> serverFiles = parseServerFileList(fileListData);
  vector<MD5File> localFiles = buildLocalFileList();

  buildFileSets(localFiles, serverFiles);

  if(downloadFiles != NULL) 
    *downloadFiles = mDownloadFiles;
  return mDownloadFiles.empty() == false || mDeleteFiles.empty() == false;
}

//--------------------------------------------------------------------------------

void
Patcher::update(ProgressListener* progressListener, bool forceNetwork)
{
  mProgressListener = progressListener;
  if(NULL == mServerConnection)
    scan(NULL, progressListener, forceNetwork);

  deleteLocalFiles();
  if(false == downloadServerFiles()) return;

  if(mDeleteEmptyDirectories)
    deleteEmptyDirectories();

  InternetCloseHandle(mServerConnection);
  mServerConnection = NULL;

  InternetCloseHandle(mInternetBase);
  mInternetBase = NULL;
}


//--------------------------------------------------------------------------------

bool
Patcher::getServerTextFile(string fileName, string& textFile)
{
  // FIXME: Handle authorization failure! 401 .. 
  HINTERNET fileRequest = HttpOpenRequest(mServerConnection, NULL, fileName.c_str(), NULL, NULL, NULL,
                                          INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD, NULL);
  if(FALSE == HttpSendRequest(fileRequest, NULL, NULL, NULL, NULL)) {
    logEngineError("Failed to open HttpRequest");
    return false;
  }
  
  char buffer[MAGIC_BUFFER_SIZE];
  ostringstream ossTextFile;
  
  BOOL readStatus = FALSE;
  DWORD bytesRead = 0;
  do {
    readStatus = InternetReadFile(fileRequest, buffer, MAGIC_BUFFER_SIZE, &bytesRead);
    if(bytesRead > 0) {
      ossTextFile << string(buffer, bytesRead);
    }
  } while(readStatus == TRUE && bytesRead != 0);
  if(readStatus == FALSE) {
    logEngineError("Error in reading file over network");
    return false;
  }
  
  CHECK("Closing handle succeeded", TRUE == InternetCloseHandle(fileRequest));
  
  textFile = ossTextFile.str();
  return true;
}


//--------------------------------------------------------------------------------

vector<MD5File>
Patcher::parseServerFileList(string fileListData)
{  
  vector<MD5File> fileList;
  if(fileListData.substr(0, 8) != "FileList") {
    logEngineError("Malformed filelist.txt");
    logEngineError(fileListData);
    return fileList;
  }
  istringstream issListStream(fileListData.substr(8, fileListData.size() - 8));

  size_t previousReadPos = 0;
  size_t currentReadPos = 0;
  while(issListStream.eof() == false) {
    MD5File newFile;
    issListStream >> newFile.md5Sum >> newFile.fileSize >> newFile.fileName;
    if(newFile.md5Sum != "" && newFile.fileName != "") 
      fileList.push_back(newFile);
    currentReadPos = issListStream.tellg();
    if(currentReadPos == previousReadPos) {
      return fileList;
    }
    previousReadPos = currentReadPos;
  }
  return fileList;
}

//--------------------------------------------------------------------------------

vector<MD5File>
Patcher::buildLocalFileList()
{
  vector<MD5File> localFiles;
  WIN32_FIND_DATA fileData;
  HANDLE searchHandle;
  
  queue<string> toScan;
  toScan.push("");
  size_t totalBytes = 0;

  while(toScan.empty() == false) {
    // Has trailing slash if needed but no leading slash
    string currentDirectory = toScan.front();
    string filePattern = mLocalPath + "/" + currentDirectory + "*";
    toScan.pop();
    searchHandle = FindFirstFile(filePattern.c_str(), &fileData);  
    if (searchHandle == INVALID_HANDLE_VALUE) continue;
    do {
      if(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if(string(fileData.cFileName) != "." && string(fileData.cFileName) != "..") {
          toScan.push(currentDirectory + fileData.cFileName + "/");
        }
      }
      else {
        MD5File newFile;
        newFile.fileName = currentDirectory + fileData.cFileName;
        newFile.fileSize = fileData.nFileSizeLow;
        localFiles.push_back(newFile);
        totalBytes += fileData.nFileSizeLow;
      }      
    } while(FindNextFile(searchHandle, &fileData));
    FindClose(searchHandle);
    mProgressListener->reportProgress(0.0);
  }
  
  // Process MD5Sums
  for(vector<MD5File>::iterator iFile = localFiles.begin(); iFile != localFiles.end(); ++iFile) {
    MD5File& file = *iFile;
    file.md5Sum = MDFile(mLocalPath + "/" + file.fileName);
    mProgressListener->reportProgress(0.0);
  }

  sort(localFiles.begin(), localFiles.end());  
  mProgressListener->reportProgress(0.0);

#ifdef _DEBUG
  OutputDebugString("Localfiles:\n");
  for(vector<MD5File>::iterator iFile = localFiles.begin(); iFile != localFiles.end(); ++iFile) {
    MD5File& file = *iFile;
    ostringstream ossDebug;
    ossDebug << file.md5Sum << " " << file.fileSize << " " << file.fileName << endl;
    OutputDebugString(ossDebug.str().c_str());
  }
#endif

  return localFiles;
}

//--------------------------------------------------------------------------------

void
Patcher::buildFileSets(vector<MD5File> localFiles, vector<MD5File> serverFiles)
{
  vector<MD5File> potentialDeleteFiles;
  vector<MD5File>::const_iterator iServerFile = serverFiles.begin();
  vector<MD5File>::const_iterator iLocalFile = localFiles.begin();  

  while(iServerFile != serverFiles.end() && iLocalFile != localFiles.end()) {
    // Delete or update, we don't know yet
    if(*iLocalFile < *iServerFile) {
      potentialDeleteFiles.push_back(*iLocalFile);
      iLocalFile++;
    }
    // File up to date
    else if(*iLocalFile == *iServerFile) {
      iServerFile++;
      iLocalFile++;
    }
    // Definitely update
    else {
      mDownloadFiles.push_back(*iServerFile);
      iServerFile++;
    }
  }
  // Push the tails correctly
  while(iServerFile != serverFiles.end()) mDownloadFiles.push_back(*iServerFile++);
  while(iLocalFile != localFiles.end()) potentialDeleteFiles.push_back(*iLocalFile++);

  sort(potentialDeleteFiles.begin(), potentialDeleteFiles.end(), lessMD5Name);
  sort(mDownloadFiles.begin(), mDownloadFiles.end(), lessMD5Name);

#ifdef _DEBUG
  OutputDebugString("\nDownloadFiles:\n");
  for(vector<MD5File>::iterator iFile = mDownloadFiles.begin(); iFile != mDownloadFiles.end(); ++iFile) {
    MD5File& file = *iFile;
    ostringstream ossDebug;
    ossDebug << file.md5Sum << " " << file.fileSize << " " << file.fileName << endl;
    OutputDebugString(ossDebug.str().c_str());
  }
#endif
  
  mDeleteFiles.resize(potentialDeleteFiles.size());
  vector<MD5File>::iterator iDeleteFiles = set_difference(potentialDeleteFiles.begin(), potentialDeleteFiles.end(),
                                                          mDownloadFiles.begin(), mDownloadFiles.end(),
                                                          mDeleteFiles.begin(), lessMD5Name);
  mDeleteFiles.resize(iDeleteFiles - mDeleteFiles.begin());

#ifdef _DEBUG
  OutputDebugString("\nDeleteFiles:\n");
  for(vector<MD5File>::iterator iFile = mDeleteFiles.begin(); iFile != mDeleteFiles.end(); ++iFile) {
    MD5File& file = *iFile;
    ostringstream ossDebug;
    ossDebug << file.md5Sum << " " << file.fileSize << " " << file.fileName << endl;
    OutputDebugString(ossDebug.str().c_str());
  }
#endif

}


//--------------------------------------------------------------------------------
void
Patcher::deleteLocalFiles()
{
  for(vector<MD5File>::const_iterator iDeleteFile = mDeleteFiles.begin(); iDeleteFile != mDeleteFiles.end(); ++iDeleteFile) {
    string fileName = (*iDeleteFile).fileName;
    string localFileName = mLocalPath + "/" + fileName;
    DeleteFile(localFileName.c_str());
  }
}


//--------------------------------------------------------------------------------

bool
Patcher::downloadServerFiles()
{  
  size_t totalDownload = 0;
  for(vector<MD5File>::const_iterator iDownloadFile = mDownloadFiles.begin(); iDownloadFile != mDownloadFiles.end(); ++iDownloadFile) {
    totalDownload += (*iDownloadFile).fileSize;
  }

  size_t bytesDownloaded = 0;
  for(vector<MD5File>::const_iterator iDownloadFile = mDownloadFiles.begin(); iDownloadFile != mDownloadFiles.end(); ++iDownloadFile) {
    const MD5File& downloadFile = *iDownloadFile;

    string fileName = downloadFile.fileName;
    string filePath =  mRemotePath + "/" + fileName;

#ifdef _DEBUG
    ostringstream ossTest;
    ossTest << fileName << " " << downloadFile.fileSize
            << " " << static_cast<int>(100 * bytesDownloaded / totalDownload) << "% " << bytesDownloaded/1000 << "k/" << totalDownload/1000 << "k" << endl;
    OutputDebugString(ossTest.str().c_str());
#endif

    HINTERNET fileRequest = HttpOpenRequest(mServerConnection, NULL, filePath.c_str(), NULL, NULL, NULL,
                                            INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD, NULL);
    if(FALSE == HttpSendRequest(fileRequest, NULL, NULL, NULL, NULL)) return false;
   
    string localFileName = mLocalPath + "/" + fileName;

    // Create all directories on the way
    unsigned int iDirOffset = 0;
    iDirOffset = localFileName.find("/", iDirOffset);
    while(iDirOffset != string::npos) {
      string directoryPart = localFileName.substr(0, iDirOffset);
      CreateDirectory(directoryPart.c_str(), NULL);
      iDirOffset = localFileName.find("/", iDirOffset + 1);
    }
    
    // Open the file itself
    FILE* localFile = fopen(localFileName.c_str(), "wbS");

    BYTE buffer[MAGIC_BUFFER_SIZE];
    BOOL readStatus = FALSE;
    DWORD bytesRead = 0;
    do {
      readStatus = InternetReadFile(fileRequest, buffer, MAGIC_BUFFER_SIZE, &bytesRead);
      if(bytesRead > 0) {
        if(mProgressListener != NULL)
          mProgressListener->reportProgress(bytesRead);
        fwrite(buffer, 1, bytesRead, localFile);
      }
    } while(readStatus == TRUE && bytesRead != 0);
    if(readStatus == FALSE) {
      return false;
    }
    fclose(localFile);
    InternetCloseHandle(fileRequest);

    bytesDownloaded += downloadFile.fileSize;
  }  
  return true;
}

//--------------------------------------------------------------------------------

bool
Patcher::isDirectoryEmpty(string directoryPath, vector<string>& emptyDirectories)
{
  WIN32_FIND_DATA fileData;
  HANDLE searchHandle;
  
  string filePattern = directoryPath + "/*";
  int nonEmptyFiles = 0;
  searchHandle = FindFirstFile(filePattern.c_str(), &fileData);  
  if (searchHandle == INVALID_HANDLE_VALUE) return false;
  do {
    if(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if(string(fileData.cFileName) != "." && string(fileData.cFileName) != "..") {
        if(isDirectoryEmpty(directoryPath + "/" + fileData.cFileName, emptyDirectories) == false)
          nonEmptyFiles++;
      }
    }
    else {
      nonEmptyFiles++;
    }
  } while(FindNextFile(searchHandle, &fileData));
  FindClose(searchHandle);

  if(nonEmptyFiles == 0) {
    emptyDirectories.push_back(directoryPath);
    return true;
  }
  else {
    return false;
  }
}


void
Patcher::deleteEmptyDirectories()
{
  vector<string> emptyDirectories;
  isDirectoryEmpty(mLocalPath, emptyDirectories);
  
  for(vector<string>::iterator iDirectory = emptyDirectories.begin(); iDirectory != emptyDirectories.end(); ++iDirectory) {
    string currentDirectory = *iDirectory;
//     string debugMessage = currentDirectory + "\n";
//     OutputDebugString(debugMessage.c_str());
    RemoveDirectory(currentDirectory.c_str());
  }
}


//================================================================================
// Update test to find out how the update mechanism works

TEST_FIXTURE_DEFINITION("update/drm/Patcher", TestPatcherUpdate);

void
TestPatcherUpdate::update_test_directory()
{
  Patcher* patcher = new Patcher("testdata/TestFilePatcherUpdate/assets", "trunk/assets_full");
  patcher->scan();
  patcher->update();
}
