//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"
#include "CacheManager.hpp"
#include "ChunkData.hpp"
#include "ChunkStream.hpp"

#include "drm/UpdateManager.hpp"
#include "main/Platform.hpp"
#include "main/Timer.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

CacheManager*
rCacheManager()
{
  static CacheManager* mCacheManager = new CacheManager();
  return mCacheManager;
}

// --------------------------------------------------------------------------------

// Forcing rebuild at 80% of coverage
const float CACHE_REBUILD_THRESHOLD = 0.8f;

CacheManager::CacheManager()
  : mCacheInfo(NULL), mCacheData(NULL), mCacheSize(0), mCacheCoverage(0.0f)
{
  if(!aquireCache()) {
    CHECK("Update in progress, fatal error", !rUpdateManager()->isActive());
    rebuildCache();
    CHECK("Aquiring cache after rebuild failed", aquireCache());
  }
}

CacheManager::~CacheManager()
{
  releaseCache();
}


// --------------------------------------------------------------------------------

bool
CacheManager::lazyRebuild()
{
  if(!rUpdateManager()->isActive()) {
    if(CACHE_REBUILD_THRESHOLD > mCacheCoverage) {
      CHECK("CacheManager::lazyRebuild(): releaseCache", releaseCache());
      CHECK("CacheManager::lazyRebuild(): rebuildCache", rebuildCache());
      CHECK("CacheManager::lazyRebuild(): aquireCache", aquireCache());
      return true;
    }
  }
  return false;
}


// --------------------------------------------------------------------------------

bool
CacheManager::aquireChunkData(string fileName, ChunkData*& oChunkData)
{
  CacheFileIterator iCache = mCacheFiles.find(fileName);
  if(iCache == mCacheFiles.end()) {
    logEngineError("CacheManager::aquireChunkData: The current CacheManager version does not support aquiring files add after initialization.");
    return false;
  }
  CacheFile& cacheFile = (*iCache).second;
  if(cacheFile.referenceCount == 0) {
    if(!rUpdateManager()->isActive()) {
      // A file can only move out from the cache
      if(cacheFile.inCache && isFileStale(cacheFile)) {
        cacheFile.inCache = false;
        cacheFile.data = NULL;
      }
      // A file can pass through both if clauses if it gets evicted
      if(!cacheFile.inCache && cacheFile.data == NULL) {
        cacheFile.data = rPlatform()->aquireFileMapping(cacheFile.fileName, cacheFile.fileSize);
        if(NULL == cacheFile.data) {
          logEngineError(string("CacheManager::aquireChunkData: Unable to aquire file mapping for ") + cacheFile.fileName);
          return false;
        }
      }
    }
    else if(!cacheFile.inCache) {
      logEngineWarn("CacheManager::aquireChunkData: Unable to aquire external files when update is running");
      return false;
    }
    cacheFile.chunkData = new ChunkData(static_cast<int8u*>(cacheFile.data), cacheFile.fileSize);
  }
  cacheFile.referenceCount++;
  oChunkData = cacheFile.chunkData;
  return true;
}

// --------------------------------------------------------------------------------

bool
CacheManager::releaseChunkData(string fileName)
{
  CacheFileIterator iCache = mCacheFiles.find(fileName);
  if(iCache == mCacheFiles.end()) {
    logEngineWarn("CacheManager::releaseChunkData: Trying to release nonexistent file");
    return false;
  }
  CacheFile& cacheFile = (*iCache).second;
  if(0 == cacheFile.referenceCount) {
    logEngineWarn("CacheManager::releaseChunkData: Trying to release file with reference count of 0");
    return false;
  }
  cacheFile.referenceCount--;
  if(0 == cacheFile.referenceCount) {
    if(NULL == cacheFile.chunkData) {
      logEngineWarn("CacheManager::releaseChunkData: Trying to release NULL ChunkData");
      return false;
    }

    if(false == cacheFile.inCache) {
      rPlatform()->releaseFileMapping(cacheFile.fileName);
    }    
    
    delete cacheFile.chunkData;
    cacheFile.chunkData = NULL;
  }
  
  return true;
}

// --------------------------------------------------------------------------------

const map<string, CacheFile>&
CacheManager::getAllFiles()
{
  return mCacheFiles;
}


// --------------------------------------------------------------------------------
bool
CacheManager::isCacheAquired()
{
  return mCacheData != NULL && mCacheInfo != NULL;
}

// --------------------------------------------------------------------------------

bool
CacheManager::aquireCache()
{
  CHECK("aquireCache duplicate", !isCacheAquired());

  // FIXME: Some better way to get profile information
  Timer* scanTimer = rPlatform()->createTimer();
  scanTimer->start();
  
  size_t fileSize;
  void* infoData = rPlatform()->aquireFileMapping("cache.shd", fileSize);
  if(NULL == infoData) {
    logEngineError("CacheManager::aquireCache(): Unable to aquire file mapping for cache.shd");
    releaseCache();
    return false;
  }
  mCacheInfo = new ChunkData(static_cast<int8u*>(infoData), fileSize);
  if(!mCacheInfo->isSignatureValid()) {
    logEngineError("CacheManager::aquireCache() error: cache.shd has invalid signature");
    releaseCache();
    return false;
  }
  ChunkIterator iChunk = mCacheInfo->begin();
  while(iChunk != mCacheInfo->end() && iChunk.getChunkType() != "FEND" ) {
    if(!iChunk.checkCRC()) {
      releaseCache();
      logEngineError(string("CacheManager::aquireCache() error: cache.shd chunk ") + iChunk.getChunkType() + " has invalid CRC");
      return false;
    }
    ++iChunk;
  }
  if(iChunk == mCacheInfo->end() || iChunk.getChunkType() != "FEND" || iChunk.getChunkSize() != 0 || iChunk.checkCRC() == false) {
    releaseCache();
    logEngineError("CacheManager::aquireCache() error: cache.shd missing or corrupted FEND chunk");
    return false;
  }
  ++iChunk;
  if(iChunk != mCacheInfo->end()) {
    releaseCache();
    logEngineError("CacheManager::aquireCache() error: extra data after FEND chunk");
    return false;
  }
  
  mCacheData = rPlatform()->aquireFileMapping("cache.dat", mCacheSize);
  if(NULL == mCacheData) {
    releaseCache();
    logEngineError("CacheManager::aquireCache() error: unable to aquire cache.dat");
    return false;
  }

  size_t fileSizeTotal = 0;
  size_t fileSizeCache = 0;

  // First read which files are in the cache
  iChunk = mCacheInfo->begin();
  while(iChunk != mCacheInfo->end() && iChunk.getChunkType() != "FEND") {
    if(iChunk.getChunkType() != "FILE") {
      ostringstream ossError;
      ossError << "CacheManager::aquireCache: skipping unknown chunk: " << iChunk.getChunkType() << " " << iChunk.getChunkSize() << " bytes";
      logEngineWarn(ossError.str());
      ++iChunk;
      continue;
    }

    CacheFile cacheFile;
    iChunk.read(cacheFile.timeStampLow);
    iChunk.read(cacheFile.timeStampHigh);
    size_t cacheOffset;
    iChunk.read(cacheOffset);
    cacheFile.data = static_cast<void*>(static_cast<char*>(mCacheData) + cacheOffset);
    if(cacheOffset != fileSizeCache) {
      ostringstream ossError;
      ossError << "CacheManager::aquireCache() error: Misaligned at " << cacheOffset << " expected " << fileSizeCache;
      logEngineError(ossError.str());
      releaseCache();
      return false;
    }
    
    iChunk.read(cacheFile.fileSize);
    iChunk.readString(cacheFile.fileName);
    cacheFile.inCache = true;
    cacheFile.referenceCount = 0;
    cacheFile.chunkData = NULL;
    ++iChunk;

    fileSizeTotal += cacheFile.fileSize;
    fileSizeCache += cacheFile.fileSize;
    
    if(mCacheFiles.find(cacheFile.fileName) != mCacheFiles.end()) {
      ostringstream ossError;
      ossError << "CacheManager::aquireCache() error: Duplicate cache file entry " << cacheFile.fileName;
      logEngineError(ossError.str());
      releaseCache();
      return false;
    }
    
    // DEBUG
    ostringstream ossInfo;
    ossInfo << cacheFile.fileName << " " << cacheFile.fileSize;
    logEngineInfo(string("CacheFile: ") + ossInfo.str());

    mCacheFiles[cacheFile.fileName] = cacheFile;
  }
  
  if(fileSizeCache != mCacheSize) {
      ostringstream ossError;
      ossError << "CacheManager::aquireCache() error: Cache file size not equal found " << fileSizeCache << " expected " << mCacheSize;
      logEngineError(ossError.str());
      releaseCache();
      return false;
  }
  

  // Inhibit local file checking as it might risk a collision with the update process
  if(rUpdateManager()->isActive()) 
    return true;

  // Now check which local files might be newer than the ones we have around
  // FIX: We should revert to the one in cache
  vector<FileEntry> allFiles = rPlatform()->scanAllFiles();
  for(vector<FileEntry>::iterator iFile = allFiles.begin(); iFile != allFiles.end(); ++iFile) {
    const FileEntry& fileEntry = *iFile;
    if(mCacheFiles.find(fileEntry.fileName) == mCacheFiles.end()) {
      CacheFile diskFile;
      diskFile.fileName = fileEntry.fileName;
      diskFile.inCache = false;
      mCacheFiles[diskFile.fileName] = diskFile;
    }
    
    CacheFile& cacheFile = mCacheFiles[fileEntry.fileName];

    // Cache file differs from the one on disk
    if(cacheFile.inCache == false || // Catch those not in cache at all      
       isFileStale(cacheFile, fileEntry)) { 
      
      // Scan header and see if the file is valid
      if(!scanHeader(cacheFile)) {
        // New file was corrupted, simply leave it out
        if(!cacheFile.inCache) {
          mCacheFiles.erase(fileEntry.fileName);
          fileSizeTotal -= fileEntry.fileSize;
        }
        // else / The file on disk was corrupted but we have it in the cache. Trust the cache
        continue;
      }
      
      if(cacheFile.inCache) {
        fileSizeCache -= cacheFile.fileSize;
        fileSizeTotal -= cacheFile.fileSize;
      }
      fileSizeTotal += fileEntry.fileSize;
      
      cacheFile.fileSize = fileEntry.fileSize;
      cacheFile.timeStampLow = fileEntry.timeStampLow;
      cacheFile.timeStampHigh = fileEntry.timeStampHigh;
      
      cacheFile.inCache = false;
      cacheFile.referenceCount = 0;
      cacheFile.chunkData = NULL;
      cacheFile.data = NULL;      
    }    
  }  
  
  mCacheCoverage = static_cast<float>(fileSizeCache) / static_cast<float>(fileSizeTotal);  

  int ms = scanTimer->check(1000);
  ostringstream ossInfo;
  ossInfo << "Cache aquire: " << ms << " ms";
  logEngineInfo(ossInfo.str());

  return true;
}

// --------------------------------------------------------------------------------


bool
CacheManager::releaseCache()
{
  for(map<string, CacheFile>::iterator iFile = mCacheFiles.begin(); iFile != mCacheFiles.end(); ++iFile) {
    CacheFile& cacheFile = (*iFile).second;
    if(cacheFile.referenceCount > 0) {
      ostringstream ossError;
      if(!cacheFile.inCache) {
        ossError << "CacheManager::releaseCache: unreleased external file: " << cacheFile.fileName << " refcount: " << cacheFile.referenceCount;
        logEngineWarn(ossError.str());
        rPlatform()->releaseFileMapping(cacheFile.fileName);
      }
      else {
        ossError << "CacheManager::releaseCache: unreleased cache file: " << cacheFile.fileName << " refcount: " << cacheFile.referenceCount;
        logEngineWarn(ossError.str());
      }      
    }
    if(cacheFile.chunkData != NULL) {
      logEngineWarn("CacheManage::releaseCache: Stale ChunkData pointer");
      delete cacheFile.chunkData;
    }    
    cacheFile.chunkData = NULL;
    cacheFile.data = NULL;
  }
  mCacheFiles.clear();
  
  rPlatform()->releaseFileMapping("cache.shd");
  rPlatform()->releaseFileMapping("cache.dat");
  if(mCacheInfo != NULL)
    delete mCacheInfo;
  mCacheInfo = NULL;
  mCacheData = NULL;
  mCacheSize = 0;
  mCacheCoverage = 0;
  
  return true;
}


// --------------------------------------------------------------------------------

bool
CacheManager::rebuildCache()
{
  Timer* scanTimer = rPlatform()->createTimer();
  scanTimer->start();

  ChunkStream* infoStream = new ChunkStream("cache.shd");
  ofstream cacheFile("cache.dat", ios_base::trunc | ios_base::binary);

  size_t cacheSize = 0;
  vector<FileEntry> allFiles = rPlatform()->scanAllFiles();
  for(vector<FileEntry>::iterator iFile = allFiles.begin(); iFile != allFiles.end(); ++iFile) {
    const FileEntry& entry = *iFile;
    
    size_t fileSize;
    void* data = rPlatform()->aquireFileMapping(entry.fileName, fileSize);
    if(NULL == data || 0 == fileSize) {
      logEngineWarn("CacheManager::rebuild(): Unable to aquire file mapping");
      continue;
    }
    ChunkData* chunkData = new ChunkData(static_cast<int8u*>(data), fileSize); 
    if(!chunkData->checkAll()) {
      ostringstream ossError;
      ossError << "CacheManager::rebuild(): corrupted file: " << entry.fileName;
      logEngineError(ossError.str());
      delete chunkData;
      rPlatform()->releaseFileMapping(entry.fileName);
      continue;
    }
    
    // Write our standard file Chunk.
    infoStream->openChunk("FILE");
    infoStream->write(entry.timeStampLow);
    infoStream->write(entry.timeStampHigh);
    infoStream->write(cacheSize);
    infoStream->write(fileSize);
    infoStream->writeString(entry.fileName);
    infoStream->closeChunk();    

    // OK, we have a valid SHD file, first write the file into the cache
    cacheFile.write(static_cast<const char*>(data), fileSize);
    cacheSize += fileSize;
    rPlatform()->releaseFileMapping(entry.fileName);
  }

  infoStream->close();
  cacheFile.close();

  int ms = scanTimer->check(1000);
  float sizeMB = (cacheSize / 1000000.0f);
  float MBperSec = sizeMB / (ms / 1000.0);
  ostringstream ossInfo;
  ossInfo << "Cache rebuild: " << fixed << setprecision(2) << static_cast<int>(cacheSize)
          << " bytes in " << ms << " ms with speed " << MBperSec << " MB/sec";
  logEngineInfo(ossInfo.str());
  return true;
}

// --------------------------------------------------------------------------------

bool
CacheManager::scanHeader(CacheFile& cacheFile)
{
  size_t fileSize;
  void* data = rPlatform()->aquireFileMapping(cacheFile.fileName, fileSize);
  if(NULL == data || 0 == fileSize) {
    logEngineWarn("CacheManager::scanHeader(): Unable to aquire file mapping");
    return false;
  }    

  ChunkData* chunkData = new ChunkData(static_cast<int8u*>(data), fileSize);
  if(!chunkData->isSignatureValid()) {
    ostringstream ossError;
    ossError << "CacheManager::scanHeader(): corrupted file: " << cacheFile.fileName;
    logEngineError(ossError.str());
    delete chunkData;
    rPlatform()->releaseFileMapping(cacheFile.fileName);
    return false;
  }      
  
  delete chunkData;
  rPlatform()->releaseFileMapping(cacheFile.fileName);        
  return true;
}

// --------------------------------------------------------------------------------



bool
CacheManager::isFileStale(const CacheFile& cacheFile, const FileEntry& fileEntry)
{
  if(fileEntry.timeStampHigh > cacheFile.timeStampHigh ||
     (fileEntry.timeStampHigh == cacheFile.timeStampHigh &&
      fileEntry.timeStampLow > cacheFile.timeStampLow) ||
     fileEntry.fileSize != cacheFile.fileSize) {
    return true;
  }
  else {
    return false;
  }
}

bool
CacheManager::isFileStale(const CacheFile& cacheFile)
{
  FileEntry fileEntry;
  if(!rPlatform()->scanFile(cacheFile.fileName, fileEntry) ) {
    logEngineError("CacheManager::isFileStale unable to scan file");
    return false;
  }
  else {
    return isFileStale(cacheFile, fileEntry);
  }
}
