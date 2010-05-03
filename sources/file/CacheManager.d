//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import ChunkData;
private import ChunkStream;
private import ChunkCommon;
private import Platform;
private import std.file;
private import std.stream;

// --------------------------------------------------------------------------------

// FIXME: This should go someplace real
class UpdateManager
{
  bool isActive()
  {
    return false;
  }  
}

private UpdateManager gUpdateManager = null;

UpdateManager
rUpdateManager()
{
  if(gUpdateManager is null)
    gUpdateManager = new UpdateManager();
  return gUpdateManager;
}


// --------------------------------------------------------------------------------

struct CacheFile
{
  char[] fileName;
  uint fileSize;
  long timeStampLow;
  long timeStampHigh;

  bool inCache;
  int referenceCount;
  ChunkData chunkData;
  ubyte[] memory;
  MmFile fileMapping;
}




class CacheManager
{


  // --------------------------------------------------------------------------------

  // Forcing rebuild at 80% of coverage
  const float CACHE_REBUILD_THRESHOLD = 0.8f;
  const uint CACHE_ENDMARKER_LENGTH = 4;

  this()
  {
    mCacheInfo = null;
    mCacheInfoFile = null;
    mCacheFile = null;
    mCacheCoverage = 0;

    if(!aquireCache()) {
      assert(CHECK("Update in progress, fatal error", !rUpdateManager().isActive()));
      rebuildCache();
      assert(CHECK("Aquiring cache after rebuild failed", aquireCache()));
    }
  }

  ~this()
  {
    releaseCache();
  }


  // --------------------------------------------------------------------------------

  bool
  lazyRebuild()
  {
    if(!rUpdateManager().isActive()) {
      if(CACHE_REBUILD_THRESHOLD > mCacheCoverage) {
        assert(CHECK("CacheManager::lazyRebuild(): releaseCache", releaseCache()));
        assert(CHECK("CacheManager::lazyRebuild(): rebuildCache", rebuildCache()));
        assert(CHECK("CacheManager::lazyRebuild(): aquireCache", aquireCache()));
        return true;
      }
    }
    return false;
  }


  // --------------------------------------------------------------------------------

  bool
  aquireChunkData(char[] fileName, out ChunkData oChunkData)
  {
    CacheFile* cacheFile = fileName in mCacheFiles;

    if(cacheFile is null) {
      logError << "CacheManager::aquireChunkData: " << fileName << " aquiring files added after initialization not supported" << endl;
      return false;
    }
    if(cacheFile.referenceCount == 0) {
      if(!rUpdateManager().isActive()) {
        // A file can only move out from the cache
        if(cacheFile.inCache && isFileStale(*cacheFile)) {
          cacheFile.inCache = false;
          cacheFile.memory = null;
        }
        // A file can pass through both if clauses if it gets evicted
        if(!cacheFile.inCache && cacheFile.memory is null) {
          try {
            cacheFile.fileMapping = new MmFile(cacheFile.fileName);
          }
          catch(FileException e) {
            logError << "CacheManager::aquireChunkData: Unable to aquire file mapping for " <<  cacheFile.fileName << endl
                     << " " << e.toString() << endl;
            return false;
          }
          cacheFile.memory = cast(ubyte[])cacheFile.fileMapping[];
        }
      }
      else if(!cacheFile.inCache) {
        logWarn << "CacheManager::aquireChunkData: Unable to aquire external files when update is running" << endl;
        return false;
      }
      cacheFile.chunkData = new ChunkData(cacheFile.memory);
    }
    cacheFile.referenceCount++;
    oChunkData = cacheFile.chunkData;
    return true;
  }

  // --------------------------------------------------------------------------------

  bool
  releaseChunkData(char[] fileName)
  {
    // CacheRelease is a nop.. We only release after run.

//     CacheFile* cacheFile = fileName in mCacheFiles;

//     if(cacheFile is null) {
//       logWarn << "CacheManager::releaseChunkData: Trying to release nonexistent file" << endl;
//       return false;
//     }
//     if(0 == cacheFile.referenceCount) {
//       logWarn << "CacheManager::releaseChunkData: Trying to release file with reference count of 0" << endl;
//       return false;
//     }
//     cacheFile.referenceCount--;
//     if(0 == cacheFile.referenceCount) {
//       if(cacheFile.chunkData is null) {
//         logWarn << "CacheManager::releaseChunkData: Trying to release null ChunkData" << endl;
//         return false;
//       }

//       if(false == cacheFile.inCache) {
//         if(cacheFile.fileMapping !is null)
//           delete cacheFile.fileMapping;
//       }    
    
//       cacheFile.chunkData = null;
//     }
  
    return true;
  }

  // --------------------------------------------------------------------------------

  CacheFile[char[]]
  getAllFiles()
  {
    return mCacheFiles;
  }


  // --------------------------------------------------------------------------------
  bool
  isCacheAquired()
  {
    return cast(bool)(mCacheFile !is null && mCacheInfoFile !is null && mCacheInfo !is null);
  }

  // --------------------------------------------------------------------------------

  bool
  aquireCache()
  {
    assert(CHECK("aquireCache duplicate", !isCacheAquired()));

    // FIXME: Some better way to get profile information
    Timer scanTimer = rPlatform().createTimer();
    scanTimer.start();
  
    uint fileSize;
    try {
      mCacheInfoFile = new MmFile("cache.shd");
    } catch(FileException e) {
      logError << "CacheManager::aquireCache(): Unable to aquire file mapping for cache.shd" << endl
               << " " << e.toString << endl;
      releaseCache();
      return false;
    }
    mCacheInfo = new ChunkData(cast(ubyte[])mCacheInfoFile[]);
    if(!mCacheInfo.isSignatureValid()) {
      logError << "CacheManager::aquireCache() error: cache.shd has invalid signature" << endl;
      releaseCache();
      return false;
    }
    ChunkIterator iChunk = mCacheInfo.begin();
    while(iChunk != mCacheInfo.end() && iChunk.getChunkType() != "FEND") {    
      if(!iChunk.checkCRC()) {
        logError << "CacheManager::aquireCache() error: cache.shd chunk " << iChunk.getChunkType() << " has invalid CRC" << endl;
        releaseCache();
        return false;
      }
      iChunk++;
    }
    if(iChunk == mCacheInfo.end() || iChunk.getChunkType() != "FEND" || iChunk.getChunkSize() != 0 || iChunk.checkCRC() == false) {
      logError << "CacheManager::aquireCache() error: cache.shd missing or corrupted FEND chunk" << endl;
      releaseCache();
      return false;
    }
    iChunk++;
    if(iChunk != mCacheInfo.end()) {
      logError << "CacheManager::aquireCache() error: extra data after FEND chunk" << endl;
      releaseCache();
      return false;
    }
  
    try {
      mCacheFile = new MmFile("cache.dat");
    } catch(FileException e) {
      logError << "CacheManager::aquireCache() error: unable to aquire cache.dat" << endl
               << " " << e.toString << endl;
      releaseCache();
      return false;
    }

    uint fileSizeTotal = 0;
    uint fileSizeCache = 0;

    // First read which files are in the cache
    iChunk = mCacheInfo.begin();
    while(iChunk != mCacheInfo.end() && iChunk.getChunkType() != "FEND") {
      if(iChunk.getChunkType() != "FILE") {
        logWarn << "CacheManager::aquireCache: skipping unknown chunk: " << iChunk.getChunkType() << " " << iChunk.getChunkSize() << " bytes" << endl;
        iChunk++;
        continue;
      }

      CacheFile cacheFile;
      iChunk.read(cacheFile.timeStampLow);
      iChunk.read(cacheFile.timeStampHigh);
      uint cacheOffset;
      iChunk.read(cacheOffset);
      iChunk.read(cacheFile.fileSize);
      iChunk.readString(cacheFile.fileName);

      cacheFile.memory = cast(ubyte[])mCacheFile[cacheOffset .. cacheOffset + cacheFile.fileSize];
      if(cacheOffset != fileSizeCache) {
        logError << "CacheManager::aquireCache() error: Misaligned at " << cacheOffset << " expected " << fileSizeCache << endl;
        releaseCache();
        return false;
      }
    
      cacheFile.inCache = true;
      cacheFile.referenceCount = 0;
      cacheFile.chunkData = null;
      iChunk++;

      fileSizeTotal += cacheFile.fileSize;
      fileSizeCache += cacheFile.fileSize;
    
      if(cacheFile.fileName in mCacheFiles !is null) {
        logError << "CacheManager::aquireCache() error: Duplicate cache file entry " << cacheFile.fileName << endl;
        releaseCache();
        return false;
      }
    
      // DEBUG
      logInfo << "CacheFile: " << cacheFile.fileName << " " << cacheFile.fileSize << endl;

      mCacheFiles[cacheFile.fileName] = cacheFile;
    }
  
    if(fileSizeCache != (mCacheFile.length - CACHE_ENDMARKER_LENGTH)) {
      logError << "CacheManager::aquireCache() error: Cache file size not equal found " << fileSizeCache << " expected " << cast(uint)mCacheFile.length << endl;
      releaseCache();
      return false;
    }
  

    // Inhibit local file checking as it might risk a collision with the update process
    if(rUpdateManager().isActive()) 
      return true;

    // Now check which local files might be newer than the ones we have around
    FileEntry[] allFiles = rPlatform().scanAllFiles();
    for(int iFile = 0; iFile < allFiles.length; ++iFile) {
      FileEntry fileEntry = allFiles[iFile];
      
      CacheFile* cacheFile = fileEntry.fileName in mCacheFiles;
      // Insert all non-cache files
      if(cacheFile is null) {
        cacheFile = new CacheFile;
        cacheFile.fileName = fileEntry.fileName;
        cacheFile.inCache = false;
        mCacheFiles[cacheFile.fileName] = *cacheFile;
        fileSizeTotal += fileEntry.fileSize;
      }
      // File found in cache
      else if(isFileStale(*cacheFile, fileEntry)) {
        fileSizeCache -= cacheFile.fileSize;
        fileSizeTotal -= cacheFile.fileSize;
        fileSizeTotal += fileEntry.fileSize;
      
        cacheFile.fileSize = fileEntry.fileSize;
        cacheFile.timeStampLow = fileEntry.timeStampLow;
        cacheFile.timeStampHigh = fileEntry.timeStampHigh;
      
        cacheFile.inCache = false;
        cacheFile.referenceCount = 0;
        cacheFile.chunkData = null;
        cacheFile.memory = null;      
        cacheFile.fileMapping = null;
      }    
    }  
  
    if(fileSizeTotal == 0)
      mCacheCoverage = 1;
    else
      mCacheCoverage = cast(float)fileSizeCache / cast(float)fileSizeTotal;

    long ms = scanTimer.check(1000);
    logInfo << "Cache aquire: " << (fileSizeCache / 1000000) << "MB in "
       << ms << " ms " << cast(int)(mCacheCoverage * 100) << "% coverage" << endl;

    return true;
  }

  // --------------------------------------------------------------------------------


  bool
  releaseCache()
  {
    char[][] keys = mCacheFiles.keys;
    for(int iFile = 0; iFile < keys.length; ++iFile) {
      CacheFile cacheFile = mCacheFiles[keys[iFile]];
      if(cacheFile.referenceCount > 0) {
        if(!cacheFile.inCache) {
          logWarn << "CacheManager::releaseCache: unreleased external file: " << cacheFile.fileName << " refcount: " << cacheFile.referenceCount << endl;
          delete cacheFile.fileMapping;
          cacheFile.fileMapping = null;
        }
        else {
          logWarn << "CacheManager::releaseCache: unreleased cache file: " << cacheFile.fileName << " refcount: " << cacheFile.referenceCount << endl;
        }      
      }
      if(cacheFile.chunkData != null) {
        logWarn << "CacheManager::releaseCache: Stale ChunkData pointer" << endl;
      }    
      cacheFile.chunkData = null;
      cacheFile.memory = null;
    }

    if(mCacheFile !is null)
      delete mCacheFile;
    if(mCacheInfoFile !is null)
      delete mCacheInfoFile;
    mCacheFile = null;
    mCacheInfoFile = null;
  
    mCacheFiles = mCacheFiles.init;
    mCacheInfo = null;
    mCacheCoverage = 0;
  
    return true;
  }


  // --------------------------------------------------------------------------------

  bool
  rebuildCache()
  {
    Timer scanTimer = rPlatform().createTimer();
    scanTimer.start();

    ChunkStream infoStream = new ChunkStream("cache.shd");
    File cacheFile = new File("cache.dat", FileMode.OutNew);

    uint cacheSize = 0;
    FileEntry[] allFiles = rPlatform().scanAllFiles();
    for(int iFile = 0; iFile < allFiles.length; ++iFile) {

      FileEntry entry = allFiles[iFile];
    
      MmFile dataFile;
      try {
        dataFile = new MmFile(entry.fileName);
      }
      catch(FileException e) {
        logWarn << "CacheManager::rebuild(): Unable to aquire file mapping" << endl
                << " " << e.toString() << endl;
        continue;
      }
      ChunkData chunkData = new ChunkData(cast(ubyte[])dataFile[]); 
      if(!chunkData.checkAll()) {
        logError << "CacheManager::rebuild(): corrupted file: " << entry.fileName << endl;
        delete dataFile;
        continue;
      }
      uint fileSize = cast(uint)dataFile.length;
    
      // Write our standard file Chunk.
      infoStream.openChunk("FILE");
      infoStream.write(entry.timeStampLow);
      infoStream.write(entry.timeStampHigh);
      infoStream.write(cacheSize);
      infoStream.write(fileSize);
      infoStream.writeString(entry.fileName);
      infoStream.closeChunk();    

      // OK, now write the file into the cache. At this point we could do some fancy remapping

      cacheFile.writeExact(dataFile[].ptr, fileSize);
      cacheSize += fileSize;
      delete dataFile;
    }

    infoStream.close();
    char[] endMarker = "CEND";
    cacheFile.writeExact(endMarker.ptr, 4);
    cacheFile.close();

    long ms = scanTimer.check(1000);
    float sizeMB = (cacheSize / 1000000.0f);
    float MBperSec = sizeMB / (ms / 1000.0);
    logInfo << "Cache rebuild: " << cast(int)cacheSize << " bytes in "
            << ms << " ms with speed " << MBperSec << " MB/sec" << endl;
    return true;
  }

  // --------------------------------------------------------------------------------

//   bool
//   scanHeader(CacheFile cacheFile)
//   {
//     uint fileSize;
//     MmFile dataFile;
//     try {
//       dataFile = new MmFile(cacheFile.fileName);
// zzz
//     byte* data = rPlatform().aquireFileMapping(cacheFile.fileName, fileSize);
//     if(data is null || 0 == fileSize) {
//       logWarn << "CacheManager::scanHeader(): Unable to aquire file mapping" << endl;
//       return false;
//     }    

//     ChunkData chunkData = new ChunkData(cast(ubyte[])data[0 .. fileSize]);
//     if(!chunkData.isSignatureValid()) {
//       logError << "CacheManager::scanHeader(): corrupted file: " << cacheFile.fileName << endl;
//       rPlatform().releaseFileMapping(cacheFile.fileName);
//       return false;
//     }      
  
//     rPlatform().releaseFileMapping(cacheFile.fileName);        
//     return true;
//   }

  // --------------------------------------------------------------------------------



  bool
  isFileStale(CacheFile cacheFile, FileEntry fileEntry)
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
  isFileStale(CacheFile cacheFile)
  {
    FileEntry fileEntry;
    if(!rPlatform().scanFile(cacheFile.fileName, fileEntry) ) {
      logError << "CacheManager::isFileStale unable to scan file" << endl;
      return false;
    }
    else {
      return isFileStale(cacheFile, fileEntry);
    }
  }

  // --------------------------------------------------------------------------------

private:
  MmFile mCacheFile;
  MmFile mCacheInfoFile;
  ChunkData mCacheInfo;
  float mCacheCoverage;

  CacheFile[char[]] mCacheFiles;
}


//--------------------------------------------------------------------------------
// Singleton implementation

private CacheManager mCacheManager = null;

CacheManager
rCacheManager()
{
  if(mCacheManager is null)
    mCacheManager = new CacheManager();
  return mCacheManager;
}

