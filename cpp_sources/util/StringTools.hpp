//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef UTIL_STRING_TOOLS
#define UTIL_STRING_TOOLS

// Very simple string conversion utility that works for chars <127
inline wstring
stringToWString(string source)
{
  wstring dest;
  for(string::const_iterator charI = source.begin(); charI != source.end(); ++charI) {
    dest.push_back(static_cast<wchar_t>(*charI));
  }
  return dest;
}

inline string
wstringToString(wstring source)
{
  string dest;
  for(wstring::const_iterator charI = source.begin(); charI != source.end(); ++charI) {
    dest.push_back(static_cast<char>(*charI));
  }
  return dest;
}

#endif
