//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include <windows.h>

#include <string>
#include <sstream>

using std::string;
using std::istringstream;

//--------------------------------------------------------------------------------

void
SimpleCreateProcess(string programName, string arguments)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );
  
  CreateProcess(programName.c_str(),
                NULL,
                NULL,             // Process handle not inheritable. 
                NULL,             // Thread handle not inheritable. 
                FALSE,            // Set handle inheritance to FALSE. 
                0,                // No creation flags. 
                NULL,             // Use parent's environment block. 
                NULL,             // Use parent's starting directory. 
                &si,              // Pointer to STARTUPINFO structure.
                &pi );             // Pointer to PROCESS_INFORMATION structure.    
}

//--------------------------------------------------------------------------------
// Win32 specific wait for process function

void
waitForProcess(string processIDString)
{
  if(processIDString == "") return;
  istringstream idStream(processIDString);
  DWORD processID;
  idStream >> processID;
  HANDLE parentHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);

  DWORD exitCode;
  do {
    Sleep(100);
    GetExitCodeProcess(parentHandle, &exitCode);
  } while(exitCode == STILL_ACTIVE);
}

//--------------------------------------------------------------------------------

const int MAGIC_WAIT_SECONDS = 120;
const int MAGIC_COPY_TRIES = 10;

void
hotcopy(string source, string target = ".")
{
  WIN32_FIND_DATA fileData;
  HANDLE searchHandle;
  
  string filePattern = source + "/*";
  searchHandle = FindFirstFile(filePattern.c_str(), &fileData);  
  if (searchHandle == INVALID_HANDLE_VALUE) return;
  int totalWaitSeconds = 0;
  do {
    if(false == (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      string sourceFile = source + "/" + fileData.cFileName;
      string targetFile = target + "/" + fileData.cFileName;
      int copyTries = 0;
      while(copyTries < MAGIC_COPY_TRIES && totalWaitSeconds < MAGIC_WAIT_SECONDS && FALSE == CopyFile(sourceFile.c_str(), targetFile.c_str(), FALSE)) {
        Sleep(1000);
        totalWaitSeconds++;
        copyTries++;
      }
      // FIXME: Add proper error handling like the rest of ShortHike
//       CHECK("Withing HotCopy performance limits", copyTries < MAGIC_COPY_TRIES && totalWaitSeconds < MAGIC_WAIT_SECONDS);
    }
  } while(FindNextFile(searchHandle, &fileData));
  FindClose(searchHandle);
}

//--------------------------------------------------------------------------------

INT WINAPI WinMain(HINSTANCE , HINSTANCE, LPSTR commandLine, INT)
{
  waitForProcess(commandLine);
#ifdef _DEBUG
  hotcopy("debug_ShortHike", "testdata/TestFilePatcherUpdate");
#else
  hotcopy("binary_win32", ".");
#endif

  SimpleCreateProcess("ShortHike.exe", "");
  return 0;
}
