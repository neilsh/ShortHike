//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;

private import Main;
private import Platform;
private import EntryWin32;
private import RenderSystem;
private import DX8RenderSystem;
private import GUIManager;

private import std.c.windows.windows;
private import std.string;
private import std.file;
private import std.mmfile;
private import std.c.string;


RECT[] gMonitorBounds;

extern (Windows) 
{
  alias int (*MONITORENUMPROC)(HMONITOR, HANDLE, RECT*, int);
  
  int MonitorInfoEnumProc(HMONITOR hMonitor, HANDLE hdcMonitor, RECT* lprcMonitor, int foo) { 
    // Backup..
    //     MONITORINFOEX info;
    //     ZeroMemory(&info, info.sizeof);
    //     info.cbSize = info.sizeof;
    //     GetMonitorInfo(hMonitor, &info);    
    //     gMonitorBounds ~= info.rcMonitor; 
     gMonitorBounds ~= *lprcMonitor; 
    return TRUE; 
  }

  DWORD timeGetTime();

  // imported from commdlg.h

  const DWORD OFN_READONLY = 0x00000001;
  const DWORD OFN_OVERWRITEPROMPT = 0x00000002;
  const DWORD OFN_HIDEREADONLY = 0x00000004;
  const DWORD OFN_NOCHANGEDIR = 0x00000008;
  const DWORD OFN_SHOWHELP = 0x00000010;
  const DWORD OFN_ENABLEHOOK = 0x00000020;
  const DWORD OFN_ENABLETEMPLATE = 0x00000040;
  const DWORD OFN_ENABLETEMPLATEHANDLE = 0x00000080;
  const DWORD OFN_NOVALIDATE = 0x00000100;
  const DWORD OFN_ALLOWMULTISELECT = 0x00000200;
  const DWORD OFN_EXTENSIONDIFFERENT = 0x00000400;
  const DWORD OFN_PATHMUSTEXIST = 0x00000800;
  const DWORD OFN_FILEMUSTEXIST = 0x00001000;
  const DWORD OFN_CREATEPROMPT = 0x00002000;
  const DWORD OFN_SHAREAWARE = 0x00004000;
  const DWORD OFN_NOREADONLYRETURN = 0x00008000;
  const DWORD OFN_NOTESTFILECREATE = 0x00010000;
  const DWORD OFN_NONETWORKBUTTON = 0x00020000;
  const DWORD OFN_NOLONGNAMES = 0x00040000;
  const DWORD OFN_EXPLORER = 0x00080000;
  const DWORD OFN_NODEREFERENCELINKS = 0x00100000;
  const DWORD OFN_LONGNAMES = 0x00200000;
  const DWORD OFN_ENABLEINCLUDENOTIFY = 0x00400000;
  const DWORD OFN_ENABLESIZING = 0x00800000;
  const DWORD OFN_DONTADDTORECENT = 0x02000000;
  const DWORD OFN_FORCESHOWHIDDEN = 0x10000000;

  // imported from WinDef.h
  alias void* HMONITOR;

  // imported from WinUser.h
  export BOOL EnumDisplayMonitors(HDC hdc, RECT* lprcClip, MONITORENUMPROC lpfnEnum, LPARAM dwData);

  // imported from Windows.h
  export ATOM RegisterClassExA(WNDCLASSEXA *lpWndClass);


  struct WIN32_FILE_ATTRIBUTE_DATA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
  }  

  enum GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
  }

  export BOOL GetFileAttributesExA(LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, void* lpFileInformation);
}





// --------------------------------------------------------------------------------

class PlatformWin32 : public Platform
{
  // --------------------------------------------------------------------------------

  override bool
  openFileDialog(out char[] fileName, char[] extension)
  {
    OPENFILENAMEA openFileName;
    ZeroMemory(&openFileName, openFileName.sizeof);
    openFileName.lStructSize = openFileName.sizeof;
    const int FILEBUF_SIZE = 2048;
    char fileBuf[];
    fileBuf.length = FILEBUF_SIZE;
    fileBuf[0] = '\0';
    openFileName.lpstrFile = fileBuf;
    openFileName.nMaxFile = FILEBUF_SIZE;
    openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if(GetOpenFileNameA(&openFileName)) {
      int nameLen = strlen(fileBuf);
      fileName.length = nameLen;
      fileName[] = fileBuf[0 .. nameLen];
      return true;
    }
    else {
      return false;
    }  
  }

  // --------------------------------------------------------------------------------

  override bool
  saveFileDialog(out char[] fileName, char[] extension)
  {
    OPENFILENAMEA openFileName;
    ZeroMemory(&openFileName, openFileName.sizeof);
    openFileName.lStructSize = openFileName.sizeof;
    const int FILEBUF_SIZE = 2048;
    char fileBuf[FILEBUF_SIZE];
    fileBuf[0] = '\0';  
    openFileName.lpstrFile = fileBuf;
    openFileName.nMaxFile = FILEBUF_SIZE;
    openFileName.Flags = OFN_PATHMUSTEXIST;
    if(GetSaveFileNameA(&openFileName)) {
      fileName = fileBuf[0 .. strlen(fileBuf)];
      return true;
    }
    else {
      return false;
    }
  }

  //--------------------------------------------------------------------------------

  override void
  createProcess(char[] programName, char[] arguments)
  {    
    assert(CHECK_FAIL("Not implemented"));
    
    //     STARTUPINFO si;
    //     PROCESS_INFORMATION pi;
    
    // //     ZeroMemory( &si, sizeof(si) );
    //     si.cb = si.sizeof;
    // //     ZeroMemory( &pi, sizeof(pi) );
    
    //     CreateProcess(toStringz(programName),
    //                   toStringz(arguments),
    //                   null,             // Process handle not inheritable. 
    //                   null,             // Thread handle not inheritable. 
    //                   FALSE,            // Set handle inheritance to FALSE. 
    //                   0,                // No creation flags. 
    //                   null,             // Use parent's environment block. 
    //                   null,             // Use parent's starting directory. 
    //                   &si,              // Pointer to STARTUPINFO structure.
    //                   &pi );             // Pointer to PROCESS_INFORMATION structure.    
  }

  // --------------------------------------------------------------------------------

  override void
  openBuyPage()
  {
    //   License* currentLicense = ConfigManager::getCurrent()->getLicense();
    //   if(LICENSE_TRIAL == currentLicense->getType()) {
    //     ostringstream urlstring;
    //     urlstring << "http://www.shorthike.com/buynow.php?"
    //               << "source=trial"
    //               << "&product_ID=" << currentLicense->getProductID();
    //     ShellExecute(0, "open", urlstring.str().c_str(), NULL, NULL, SW_SHOW);
    //   }
    //   else {
    //     ostringstream urlstring;
    //     urlstring << "http://www.shorthike.com/buy.php?"
    //               << "source=trial"
    //               << "&product_ID=" << currentLicense->getProductID();
    //     ShellExecute(0, "open", urlstring.str().c_str(), NULL, NULL, SW_SHOW);
    //   }
  }

  // --------------------------------------------------------------------------------
  override Timer
  createTimer()
  {
    return new Win32Timer();
  }

  // //--------------------------------------------------------------------------------

  override void
  pumpMessages()
  {
    GUIManager guiRoot = rGUIManager();
  
    // Pump events on Win32
    MSG  msg;
    char keyCode;
    while( PeekMessageA(&msg, null, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg );
      switch(msg.message) {
      case WM_LBUTTONDOWN:
        guiRoot.injectMouseButtonEvent(0, true);
        break;
      case WM_LBUTTONUP:
        guiRoot.injectMouseButtonEvent(0, false);
        break;
      case WM_RBUTTONDOWN:
        guiRoot.injectMouseButtonEvent(1, true);
        break;
      case WM_RBUTTONUP:
        guiRoot.injectMouseButtonEvent(1, false);
        break;
      case WM_MBUTTONDOWN:
        guiRoot.injectMouseButtonEvent(2, true);
        break;
      case WM_MBUTTONUP:
        guiRoot.injectMouseButtonEvent(2, false);
        break;
      case WM_MOUSEMOVE: {
        //       POINTS currentPos = MAKEPOINTS(msg.lParam);
        int mouseX = msg.lParam & 0xffff;
        int mouseY = msg.lParam >> 16 & 0xffff;
        guiRoot.injectMouseMotionEvent(mouseX, mouseY, 0);
        break;
      }

        // FIXME: Filter out keycodes and translate them to our portable format. Also
        // filter out special control keys so they don't end up as characters .. (eg. VK_BACK)
      case WM_KEYDOWN:
        keyCode = cast(char)(msg.wParam);
        guiRoot.injectKeyboardEvent(keyCodeFromChar(keyCode), true);
        break;
      case WM_KEYUP:
        keyCode = cast(char)(msg.wParam);
        guiRoot.injectKeyboardEvent(keyCodeFromChar(keyCode), false);
        break;
      case WM_CHAR:
        wchar character = msg.wParam;
        guiRoot.injectCharacterEvent(character);
      default:
      }
      DispatchMessageA(&msg );
    }
  }

  // --------------------------------------------------------------------------------
  // Convert Win32 virtual key code into portable KeyCode members

  char[] mWin32Keys = [VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
                       VK_PRIOR, VK_NEXT, VK_ESCAPE, VK_DELETE];
  KeyCode[] mWin32KeyCodes = [KeyCode.LEFT, KeyCode.RIGHT, KeyCode.UP, KeyCode.DOWN,
                           KeyCode.PAGE_UP, KeyCode.PAGE_DOWN, KeyCode.ESCAPE, KeyCode.DELETE];

  KeyCode
  keyCodeFromChar(char key)
  {
    // Alphanum and Function
    if('a' <= key && key <= 'z')
      return KeyCode.A + (key - 'a'); 
    if('A' <= key && key <= 'Z')
      return KeyCode.A + (key - 'A');
    if('0' <= key && key <= '0')
      return KeyCode.N0 + (key - '0');
    if(VK_F1 <= key && key <= VK_F12)
      return KeyCode.F1 + (key - VK_F1);

    // Special keys
    for(int iKey = 0; iKey < mWin32Keys.length; ++iKey) {
      if(key == mWin32Keys[iKey])
        return mWin32KeyCodes[iKey];
    }

    // Nothing found
    return KeyCode.UNKNOWN;
  }
  

  //--------------------------------------------------------------------------------

  override RenderSystem
  createRenderSystem()
  {
    RenderSystem renderSystem;
    assert(CHECK("DX8RenderSystem creation succeeded", createDX8RenderSystem(&globalMessageProcedure, renderSystem)));
    return renderSystem;
  }


  // --------------------------------------------------------------------------------
  // File scanning utilities

  override FileEntry[]
  scanAllFiles()
  {
    FileEntry[] files;

    WIN32_FIND_DATA fileData;
    HANDLE searchHandle;
  
    char[][] toScan;
    toScan ~= "assets/";
  
    int iFile = 0;
    while(iFile < toScan.length) {
      // Has trailing slash if needed but no leading slash
      char[] currentDirectory = toScan[iFile];
      iFile++;

      // Scan directories
      char[] dirPattern = currentDirectory ~ "*";
      searchHandle = FindFirstFileA(toStringz(dirPattern), &fileData);  
      if (searchHandle == INVALID_HANDLE_VALUE) continue;
      do {
        if(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
          char[] cFileName = fileData.cFileName[0 .. strlen(fileData.cFileName)];
          if(cFileName != "." && cFileName != "..") {
            toScan ~= currentDirectory ~ cFileName ~ "/";
          }
        }
      } while(FindNextFileA(searchHandle, &fileData));
      FindClose(searchHandle);
    
      // Scan shd files
      char[] filePattern = currentDirectory ~ "*.shd";    
      searchHandle = FindFirstFileA(toStringz(filePattern), &fileData);  
      if (searchHandle == INVALID_HANDLE_VALUE) continue;
      do {
        FileEntry newFile;
        newFile.fileName = currentDirectory ~ fileData.cFileName[0 .. strlen(fileData.cFileName)];
        newFile.fileSize = fileData.nFileSizeLow;
        // Using write time here. Should we do creation instead?
        newFile.timeStampLow = fileData.ftLastWriteTime.dwLowDateTime;
        newFile.timeStampHigh = fileData.ftLastWriteTime.dwHighDateTime;
        files ~= newFile;
    } while(FindNextFileA(searchHandle, &fileData));
      FindClose(searchHandle);
    }
  
    return files.dup;
  }

  override bool
  scanFile(char[] fileName, out FileEntry oFileEntry)
  {
    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if(!GetFileAttributesExA(toStringz(fileName), GET_FILEEX_INFO_LEVELS.GetFileExInfoStandard, &fileData)) {
      logWarn << "Unable to scan file: " << fileName << " error: " << getLastWin32Error();
      return false;
    }
    oFileEntry.fileName = fileName;
    oFileEntry.fileSize = fileData.nFileSizeLow;
    oFileEntry.timeStampLow = fileData.ftLastWriteTime.dwLowDateTime;
    oFileEntry.timeStampHigh = fileData.ftLastWriteTime.dwHighDateTime;
    return true;
  }


  // --------------------------------------------------------------------------------
  // Dynamic library loading

  override bool
  loadLibrary(char[] fileName, out void* handle)
  {
    handle = LoadLibraryA(toStringz(fileName));
    if(handle == null)
      logError << "loadLibrary: " << getLastWin32Error() << endl;
    else
      logInfo << "loadLibrary: " << fileName << " loaded" << endl;
    return cast(bool)(handle != null);
  }
  
  override bool
  linkFunction(void* handle, char[] functionName, out void* fp)
  {
    fp = GetProcAddress(cast(HANDLE)handle, toStringz(functionName));
    if(fp == null)
      logError << "linkFunction: " << getLastWin32Error() << endl;
    return cast(bool)(fp != null);
  }
  
  override bool
  freeLibrary(void* handle)
  {
    if(!FreeLibrary(cast(HANDLE)handle)) {
      logError << "freeLibrary: " << getLastWin32Error() << endl;
      return false;
    }
    return true;
  }
}

// --------------------------------------------------------------------------------


extern(Windows)
int globalMessageProcedure(HWND window, uint message, WPARAM wParam, LPARAM lParam)
{
  switch(message) {
  case WM_ACTIVATE:
    if(WA_INACTIVE == LOWORD(wParam))
      rMain().setActive(false);
    else
      rMain().setActive(true);
    break;
  case WM_PAINT:
    rMain().render();
    break;
  case WM_NCCALCSIZE:
    // Prevent addition of caption and borders
    return 0;
  default:
  }
  return DefWindowProcA(window, message, wParam, lParam);
}

// --------------------------------------------------------------------------------

char[]
getLastWin32Error()
{
  
  HLOCAL lpMsgBuf;
  if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      null, GetLastError(),
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      cast(LPTSTR) &lpMsgBuf, 0, null )) {
    return "FAIL: Unable to retrieve Win32 error";
  }

  char* lpstring = cast(char*)lpMsgBuf;
  char[] message = lpstring[0 .. strlen(lpstring)];
  
  LocalFree(lpMsgBuf);
  return message;
}


//--------------------------------------------------------------------------------

class Win32Timer : public Timer
{
  this()
  {
    mHighFreq = QueryPerformanceFrequency(&mTicks);
    mStarted = FALSE;    
  }

  void start()
  {
    if(mHighFreq) {
      QueryPerformanceCounter(&mTimeStart);
    }
    else {
      mTimeStart = timeGetTime();
    }
    mStarted = TRUE;
  }
  
  long check(long fracSec)
  {
    if(!mStarted) return 0;
    if(mHighFreq) {
      QueryPerformanceCounter(&mTimeNow);
      return ((mTimeNow - mTimeStart) * fracSec) / mTicks;
    }
    else {
      mTimeNow = timeGetTime();
      if(mTimeNow < mTimeStart) {    //Oops, millisecond rollover.
        return ((mTimeNow + (long.max - mTimeStart)) * fracSec) / 1000;
      }
      else {
        return ((mTimeNow - mTimeStart) * fracSec) / 1000;
      }
    }
  }  


  
private:
  long mTimeStart;
  long mTicks;
  long mTimeNow;
  int mStarted;
  int mHighFreq;
};

// --------------------------------------------------------------------------------

Platform
createPlatform()
{
  PlatformWin32 platform = new PlatformWin32;
  return platform;
}

