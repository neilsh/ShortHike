//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"
#include "PlatformWin32.hpp"
#include "main/Timer.hpp"
#include "main/Main.hpp"

#include "guilib/GUIManager.hpp"
#include "rendering/RenderSystem.hpp"

// #include "Entry.hpp"
// #include "Main.hpp"
// #include "Root.hpp"
// #include "Timer.hpp"
// #include "resource.h"
// #include "rendering/RenderSystem.hpp"
// #include "guilib/GUIManager.hpp"
// #include "version.hpp"
// #include "generated/build_info.hpp"


//--------------------------------------------------------------------------------

Platform*
createPlatform()
{
  return new PlatformWin32();
}


//--------------------------------------------------------------------------------
// Force a ping of the internet to dust out any firewall dialogs

// void
// PlatformWin32::pingInternet()
// {
//   HINTERNET internetBase = InternetOpen("ShortHike", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
//   if(NULL == internetBase) return;
  
//   HINTERNET pingFile = InternetOpenUrl(internetBase, "http://www.shorthike.com", NULL, NULL,
//                                        INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD, NULL);
//   char buffer[10];
//   DWORD bytesRead;
//   InternetReadFile(pingFile, buffer, 10, &bytesRead);
  
//   InternetCloseHandle(pingFile);
//   InternetCloseHandle(internetBase);
// }

string
getLastWin32Error()
{
  
  LPVOID lpMsgBuf;
  if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     NULL, GetLastError(),
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     (LPTSTR) &lpMsgBuf, 0, NULL )) {
    return string("FAIL: Unable to retrieve Win32 error");
  }
  
  string message(static_cast<char*>(lpMsgBuf));
  
  LocalFree( lpMsgBuf );
  return message;
}


// --------------------------------------------------------------------------------

bool
PlatformWin32::openFileDialog(string& fileName)
{
  OPENFILENAME openFileName;
  ZeroMemory(&openFileName, sizeof(openFileName));
  openFileName.lStructSize = sizeof(openFileName);
  const int FILEBUF_SIZE = 2048;
  char fileBuf[FILEBUF_SIZE];
  fileBuf[0] = '\0';
  openFileName.lpstrFile = fileBuf;
  openFileName.nMaxFile = FILEBUF_SIZE;
  openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  if(GetOpenFileName(&openFileName)) {
    fileName = string(fileBuf);
    return true;
  }
  else {
    return false;
  }  
}

// --------------------------------------------------------------------------------

bool
PlatformWin32::saveFileDialog(string& fileName)
{
  OPENFILENAME openFileName;
  ZeroMemory(&openFileName, sizeof(openFileName));
  openFileName.lStructSize = sizeof(openFileName);
  const int FILEBUF_SIZE = 2048;
  char fileBuf[FILEBUF_SIZE];
  fileBuf[0] = '\0';  
  openFileName.lpstrFile = fileBuf;
  openFileName.nMaxFile = FILEBUF_SIZE;
  openFileName.Flags = OFN_PATHMUSTEXIST;
  if(GetSaveFileName(&openFileName)) {
    fileName = string(fileBuf);
    return true;
  }
  else {
    return false;
  }
}

//--------------------------------------------------------------------------------

void
PlatformWin32::createProcess(string programName, string pArguments)
{
  const int BUFFER_SIZE = 200;
  char arguments[BUFFER_SIZE];
  strncpy(arguments, pArguments.c_str(), BUFFER_SIZE);
  
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );
  
  CreateProcess(programName.c_str(),
                arguments,
                NULL,             // Process handle not inheritable. 
                NULL,             // Thread handle not inheritable. 
                FALSE,            // Set handle inheritance to FALSE. 
                0,                // No creation flags. 
                NULL,             // Use parent's environment block. 
                NULL,             // Use parent's starting directory. 
                &si,              // Pointer to STARTUPINFO structure.
                &pi );             // Pointer to PROCESS_INFORMATION structure.    
}

void
PlatformWin32::openBuyPage()
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

//--------------------------------------------------------------------------------

class Win32Timer : public Timer
{
public:
  Win32Timer()
  {
    mHighFreq = QueryPerformanceFrequency(&mTicks);
    mStarted = FALSE;    
  }

  virtual void start()
  {
    if(mHighFreq) {
      QueryPerformanceCounter(&mTimeStart);
    }
    else {
      mTimeStart.LowPart = timeGetTime();
    }
    mStarted = TRUE;
  }
  
  virtual int check(int fracSec)
  {
    if(!mStarted) return 0;
    if(mHighFreq) {
      QueryPerformanceCounter(&mTimeNow);
      return (int)(((mTimeNow.QuadPart - mTimeStart.QuadPart) * fracSec) / mTicks.QuadPart);
    }
    else {
      mTimeNow.LowPart = timeGetTime();
      if(mTimeNow.LowPart < mTimeStart.LowPart) {    //Oops, millisecond rollover.
        return (int)(((mTimeNow.LowPart + (0xffffffff - mTimeStart.LowPart)) * fracSec) / 1000);
      }
      else {
        return (int)(((mTimeNow.LowPart - mTimeStart.LowPart) * fracSec) / 1000);
      }
    }
  }  

private:
  LARGE_INTEGER mTimeStart;
  LARGE_INTEGER mTicks;
  LARGE_INTEGER mTimeNow;
  int mStarted;
  int mHighFreq;
};

Timer*
PlatformWin32::createTimer()
{
  return new Win32Timer();
}


//--------------------------------------------------------------------------------

void
PlatformWin32::pumpMessages()
{
  GUIManager* guiRoot = rGUIManager();
  
  // Pump events on Win32
  MSG  msg;
  char keyCode;
  while( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
    TranslateMessage(&msg );
    switch(msg.message) {
    case WM_LBUTTONDOWN:
      guiRoot->injectMouseButtonEvent(0, true);
      break;
    case WM_LBUTTONUP:
      guiRoot->injectMouseButtonEvent(0, false);
      break;
    case WM_RBUTTONDOWN:
      guiRoot->injectMouseButtonEvent(1, true);
      break;
    case WM_RBUTTONUP:
      guiRoot->injectMouseButtonEvent(1, false);
      break;
    case WM_MBUTTONDOWN:
      guiRoot->injectMouseButtonEvent(2, true);
      break;
    case WM_MBUTTONUP:
      guiRoot->injectMouseButtonEvent(2, false);
      break;
    case WM_MOUSEMOVE: {
      POINTS currentPos = MAKEPOINTS(msg.lParam);
      int mouseX = currentPos.x;
      int mouseY = currentPos.y;
      guiRoot->injectMouseMotionEvent(mouseX, mouseY, 0);
      break;
    }

      // FIXME: Filter out keycodes and translate them to our portable format. Also
      // filter out special control keys so they don't end up as characters .. (eg. VK_BACK)
    case WM_KEYDOWN:
      keyCode = static_cast<char>(msg.wParam);
      guiRoot->injectKeyboardEvent(keyCode, true);
      break;
    case WM_KEYUP:
      keyCode = static_cast<char>(msg.wParam);
      guiRoot->injectKeyboardEvent(keyCode, false);
      break;
    case WM_CHAR:
      wchar_t character = msg.wParam;
      guiRoot->injectCharacterEvent(character);
    }
    DispatchMessage(&msg );
  }
}

//--------------------------------------------------------------------------------

HINSTANCE currentInstance;


BOOL CALLBACK MonitorInfoEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) 
{ 
  vector<RECT>* monitorBounds = reinterpret_cast<std::vector<RECT>*>(dwData); 
  monitorBounds->push_back(*lprcMonitor); 
  return TRUE; 
} 


HWND applicationWindow;

extern(Windows)
LRESULT WINAPI globalMessageProcedure( HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
  switch(message) {
  case WM_ACTIVATE:
    if(WA_INACTIVE == LOWORD(wParam))
      rMain()->setActive(false);
    else
      rMain()->setActive(true);
    break;
  case WM_PAINT:
    rMain()->render();
    break;
  case WM_NCCALCSIZE:
    // Prevent addition of caption and borders
    return NULL;
  }
  return DefWindowProc(window, message, wParam, lParam);
}

RenderSystem*
PlatformWin32::createRenderSystem()
{
  logEngineInfo("createRenderSystem()");

  int windowWidth;
  int windowHeight;
  bool fullScreen;
  
  vector<RECT> monitorBounds; 
  EnumDisplayMonitors(NULL, NULL, MonitorInfoEnumProc, reinterpret_cast<LPARAM>(&monitorBounds));
  
  applicationWindow;
  WNDCLASSEX windowClass;
  ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
  
  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.style = CS_CLASSDC;
  windowClass.lpfnWndProc = globalMessageProcedure;
  windowClass.hInstance = GetModuleHandle(NULL);
  windowClass.hIcon = LoadIcon(currentInstance, "SHORTHIKEICON");
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  windowClass.cbWndExtra = sizeof(LONG_PTR);
  windowClass.lpszClassName = "ShortHike";

  RegisterClassEx(&windowClass);

  RECT windowSize;
  // FIXME: Read from config files.
  int displayMonitor = 0;
  displayMonitor = clamp(displayMonitor, 0, static_cast<int>(monitorBounds.size() - 1));
  windowSize = monitorBounds[displayMonitor];

  windowWidth = windowSize.right - windowSize.left;
  windowHeight = windowSize.bottom - windowSize.top;
  // Force smaller window
  windowSize.left = 20;
  windowSize.top = 20;
  windowWidth = 1024;
  windowHeight = 768;
//  Force smaller window
//   windowSize.left = 50;
//   windowSize.top = 50;
//   windowWidth = 800;
//   windowHeight = 600;
  fullScreen = false;
  
  DWORD windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
  applicationWindow = CreateWindow( "ShortHike", "ShortHike", 
                                    windowStyle, windowSize.left, windowSize.top, windowWidth, windowHeight,
                                    GetDesktopWindow(), NULL, windowClass.hInstance, NULL);

  // Separating out OGRE
//   windowParams["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)applicationWindow);  

//   renderWindow = ogreRoot->createRenderWindow("REFACTOR OGRE", windowWidth, windowHeight, fullScreen, &windowParams);
//   renderWindow->setActive(true);
//   ogreRoot->getRenderSystem()->_initRenderTargets();

//   SetWindowLongPtr(applicationWindow, 0, (LONG_PTR)renderWindow);
//   CHECK("RenderWindow as handle", GetWindowLongPtr(applicationWindow, 0) != 0);
  ShowWindow(applicationWindow, SW_SHOWNORMAL);
//   UpdateWindow(applicationWindow);

  logEngineInfo(" - Trying DX7RenderSystem");

  RenderSystem* renderSystem;
  CHECK("DX7RenderSystem creation succeeded", createDX7RenderSystem(applicationWindow, &renderSystem));
  return renderSystem;
}

// --------------------------------------------------------------------------------
// File scanning utilities

vector<FileEntry>
PlatformWin32::scanAllFiles()
{
  vector<FileEntry> files;

  WIN32_FIND_DATA fileData;
  HANDLE searchHandle;
  
  queue<string> toScan;
  toScan.push("assets/");

  while(toScan.empty() == false) {
    // Has trailing slash if needed but no leading slash
    string currentDirectory = toScan.front();
    toScan.pop();

    // Scan directories
    string dirPattern = currentDirectory + "*";
    searchHandle = FindFirstFile(dirPattern.c_str(), &fileData);  
    if (searchHandle == INVALID_HANDLE_VALUE) continue;
    do {
      if(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if(string(fileData.cFileName) != "." && string(fileData.cFileName) != "..") {
          toScan.push(currentDirectory + fileData.cFileName + "/");
        }
      }
    } while(FindNextFile(searchHandle, &fileData));
    FindClose(searchHandle);
    
    // Scan shd files
    string filePattern = currentDirectory + "*.shd";    
    searchHandle = FindFirstFile(filePattern.c_str(), &fileData);  
    if (searchHandle == INVALID_HANDLE_VALUE) continue;
    do {
      FileEntry newFile;
      newFile.fileName = currentDirectory + fileData.cFileName;
      newFile.fileSize = fileData.nFileSizeLow;
      // Using write time here. Should we do creation instead?
      newFile.timeStampLow = fileData.ftLastWriteTime.dwLowDateTime;
      newFile.timeStampHigh = fileData.ftLastWriteTime.dwHighDateTime;
      files.push_back(newFile);
    } while(FindNextFile(searchHandle, &fileData));
    FindClose(searchHandle);
  }
  
  return files;
}

bool
PlatformWin32::scanFile(string fileName, FileEntry& oFileEntry)
{
  WIN32_FILE_ATTRIBUTE_DATA fileData;
  if(!GetFileAttributesEx(fileName.c_str(), GetFileExInfoStandard, &fileData)) {
    ostringstream ossError;
    ossError << "Unable to scan file: " << fileName << " error: " << getLastWin32Error();
    logEngineWarn(ossError.str());
    return false;
  }
  oFileEntry.fileName = fileName;
  oFileEntry.fileSize = fileData.nFileSizeLow;
  oFileEntry.timeStampLow = fileData.ftLastWriteTime.dwLowDateTime;
  oFileEntry.timeStampHigh = fileData.ftLastWriteTime.dwHighDateTime;
  return true;
}


// --------------------------------------------------------------------------------

void*
failFileMapping(string method, string function, string fileName, string error = "")
{
  ostringstream ossError;
  ossError << method << ": " << " " << fileName << " error: ";
  if(error != "")
    ossError << getLastWin32Error();
  else
    ossError << error;
  logEngineError(ossError.str());  
  return NULL;
}


void*
PlatformWin32::aquireFileMapping(string fileName, size_t& oFileSize)
{
  if(mOpenFileMappings.find(fileName) == mOpenFileMappings.end()) {
    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if(!GetFileAttributesEx(fileName.c_str(), GetFileExInfoStandard, &fileData))
      return failFileMapping("aquireFileMapping", "GetFileAttributesEx", fileName);

    FileMapping mapping;
    mapping.referenceCount = 1;

    mapping.fileSize = fileData.nFileSizeLow;
    if(fileData.nFileSizeHigh > 0)
      return failFileMapping("aquireFileMapping", "WIN32_FILE_ATTRIBUTE", fileName, "File too large");    
    if(fileData.nFileSizeLow == 0)
      return failFileMapping("aquireFileMapping", "WIN32_FILE_ATTRIBUTE", fileName, "File size is zero");

    mapping.fileHandle = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(INVALID_HANDLE_VALUE == mapping.fileHandle)
      return failFileMapping("aquireFileMapping", "CreateFile", fileName);
    
    mapping.mappingHandle = CreateFileMapping(mapping.fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
    if(INVALID_HANDLE_VALUE == mapping.mappingHandle)
      return failFileMapping("aquireFileMapping", "CreateFileMapping", fileName);
    mapping.rawData = MapViewOfFile(mapping.mappingHandle, FILE_MAP_READ, 0, 0, mapping.fileSize);
    if(NULL == mapping.rawData)
      return failFileMapping("aquireFileMapping", "MapViewOfFile", fileName);
    
    // Success
    mOpenFileMappings[fileName] = mapping;
    oFileSize = mapping.fileSize;
    return mapping.rawData;
  }
  else {
    FileMapping& mapping = mOpenFileMappings[fileName];
    mapping.referenceCount++;
    oFileSize = mapping.fileSize;
    return mapping.rawData;
  }
}

void
PlatformWin32::releaseFileMapping(string fileName)
{
  if(mOpenFileMappings.find(fileName) == mOpenFileMappings.end()) {
    logEngineError("Trying to release non-existent file mapping");
  }
  else {
    FileMapping& mapping = mOpenFileMappings[fileName];
    mapping.referenceCount--;
    if(mapping.referenceCount == 0) {
      if(!UnmapViewOfFile(mapping.rawData))
        logEngineWarn(string("releaseFileMapping: UnmapViewOfFile: error: ") + getLastWin32Error());
      if(!CloseHandle(mapping.mappingHandle))
        logEngineWarn(string("releaseFileMapping: CloseHandle (File Mapping): error: ") + getLastWin32Error());
      if(!CloseHandle(mapping.fileHandle))
        logEngineWarn(string("releaseFileMapping: CloseHandle (File): error: ") + getLastWin32Error());
      mOpenFileMappings.erase(fileName);
    }
  }
}




