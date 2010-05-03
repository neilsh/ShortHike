//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

// These files are imported often enough to combine them into a main import file

import std.math;
import std.string;

import TestFixture;
import Angle;
import AxisAlignedBox;
import Matrix;
import Math;
import Quaternion;
import Ray;
import Vector;
import Log;
import Check;

import Value;


version(Windows)
{
  import std.c.windows.windows;
  import std.c.windows.com;
}

enum DiscreteResource {
  LIFESUPPORT = 0,
  STRESS,

  FLOW_INITIAL = 2,
  PASSENGERS = 2,
  VISITORS,
  CREW,
  CARGO,
  COMMODITIES,
  SERVICE
}


enum CompressionType {
  COMPRESSION_RAW = 0,
  COMPRESSION_DEFLATE = 1,
  COMPRESSION_JPEG2000 = 2
}


enum Screen {
  SHELL,
  STATION,
  MAP,
  TOOLS
}



void
ZeroMemory(void* _data, int bytes)
{
  byte* data = cast(byte*)_data;
  for(int iByte = 0; iByte < bytes; ++iByte) {
    *data++ = 0;
  }
}
