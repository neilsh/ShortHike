//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Timer;
private import RenderSystem;

version(Windows)
{
  private import PlatformWin32;
}


struct FileEntry
{
  char[] fileName;
  size_t fileSize;
  long timeStampLow;
  long timeStampHigh;
};


class Platform
{
  abstract void pumpMessages();

  abstract RenderSystem createRenderSystem();
  abstract Timer createTimer();
  abstract void createProcess(char[] programName, char[] pArguments);

  abstract void openBuyPage();

  abstract bool openFileDialog(out char[] fileName, char[] extension);
  abstract bool saveFileDialog(out char[] fileName, char[] extension);

  abstract FileEntry[] scanAllFiles();
  abstract bool scanFile(char[] fileName, out FileEntry oFileEntry);  

  abstract bool loadLibrary(char[] fileName, out void* handle);
  abstract bool linkFunction(void* handle, char[] functionName, out void* fp);
  abstract bool freeLibrary(void* handle);
}

// --------------------------------------------------------------------------------
// Singleton implementation


private Platform mPlatform;

Platform
rPlatform()
{
  if(null is mPlatform)
    mPlatform = createPlatform();
  return mPlatform;
}


