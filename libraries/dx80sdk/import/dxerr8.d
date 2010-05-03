/*==========================================================================;
 *
 *
 *  File:   dxerr8.d
 *  Content:    DirectX Error Library Include File
 *
 ****************************************************************************/

import win32api;

extern(Windows)
{

  //
  //  DXGetErrorString8
  //
  //  Desc:  Converts an DirectX HRESULT to a string
  //
  //  Args:  HRESULT hr   Can be any error code from
  //                      DPLAY D3D8 D3DX8 DMUSIC DSOUND
  //
  //  Return: Converted string
  //

  char*  DXGetErrorString8A(HRESULT hr);
  WCHAR* DXGetErrorString8W(HRESULT hr);

  version(unicode)
  {
    WCHAR* DXGetErrorString8(HRESULT hr) { return DXGetErrorString8W(hr); }
  }
  else
  {
    char* DXGetErrorString8(HRESULT hr) { return DXGetErrorString8A(hr); }
  }


  //
  //  DXTrace
  //
  //  Desc:  Outputs a formatted error message to the debug stream
  //
  //  Args:  CHAR* strFile   The current file, typically passed in using the
  //                         __FILE__ macro.
  //         DWORD dwLine    The current line number, typically passed in using the
  //                         __LINE__ macro.
  //         HRESULT hr      An HRESULT that will be traced to the debug stream.
  //         CHAR* strMsg    A string that will be traced to the debug stream (may be NULL)
  //         BOOL bPopMsgBox If TRUE, then a message box will popup also containing the passed info.
  //
  //  Return: The hr that was passed in.
  //
  HRESULT DXTraceA( char* strFile, DWORD dwLine, HRESULT hr, char* strMsg, BOOL bPopMsgBox );
  HRESULT DXTraceW( char* strFile, DWORD dwLine, HRESULT hr, WCHAR* strMsg, BOOL bPopMsgBox );

  version(unicode)
  {
    HRESULT DXTrace( char* strFile, DWORD dwLine, HRESULT hr, WCHAR* strMsg, BOOL bPopMsgBox ) { return DXTraceW(strFile, dwLine, hr, strMsg, bPopMsgBox); }
  }
  else
  {
    HRESULT DXTrace( char* strFile, DWORD dwLine, HRESULT hr, char* strMsg, BOOL bPopMsgBox ) { return DXTraceA(strFile, dwLine, hr, strMsg, bPopMsgBox); }
  }


  //
  // Helper macros
  //
  /*
  #if defined(DEBUG) | defined(_DEBUG)
      #define DXTRACE_MSG(str)              DXTrace( __FILE__, (DWORD)__LINE__, 0, str, FALSE )
      #define DXTRACE_ERR(str,hr)           DXTrace( __FILE__, (DWORD)__LINE__, hr, str, TRUE )
      #define DXTRACE_ERR_NOMSGBOX(str,hr)  DXTrace( __FILE__, (DWORD)__LINE__, hr, str, FALSE )
  #else
      #define DXTRACE_MSG(str)              (0L)
      #define DXTRACE_ERR(str,hr)           (hr)
      #define DXTRACE_ERR_NOMSGBOX(str,hr)  (hr)
  #endif
  */

}
