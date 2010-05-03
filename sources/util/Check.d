//--------------------------------------------------------------------------------
//
// ShortHike, www.shorthike.com
//
// Copyright in 2002-2006 Kai Backman, Mistaril Ltd, kai@shorthike.com
//
//--------------------------------------------------------------------------------

private import Common;

// --------------------------------------------------------------------------------

char[][char[]] gFileFromModule;

private void
gRegMod(char[] fullName)
{
  char[][] path = std.string.split(fullName, "/");
  char[] fileName = path[path.length - 1];
  gFileFromModule[fileName[0 .. fileName.length - 2]] = fullName;
}

static this()
{
  gRegMod("./libraries/dx80sdk/import/d3d8.d");
  gRegMod("./libraries/dx80sdk/import/d3d8caps.d");
  gRegMod("./libraries/dx80sdk/import/d3d8types.d");
  gRegMod("./libraries/dx80sdk/import/dxerr8.d");
  gRegMod("./libraries/dx80sdk/import/TestD3D.d");
  gRegMod("./libraries/freetype.d");
  gRegMod("./libraries/png.d");
  gRegMod("./sources/BuildInfo.d");
  gRegMod("./sources/Common.d");
  gRegMod("./sources/dx8/DX8Mesh.d");
  gRegMod("./sources/dx8/DX8RenderSystem.d");
  gRegMod("./sources/dx8/DX8Texture.d");
  gRegMod("./sources/file/CacheManager.d");
  gRegMod("./sources/file/ChunkCommon.d");
  gRegMod("./sources/file/ChunkData.d");
  gRegMod("./sources/file/ChunkStream.d");
  gRegMod("./sources/game/Defs.d");
  gRegMod("./sources/game/DiscreteComponent.d");
  gRegMod("./sources/game/DiscreteDiffuse.d");
  gRegMod("./sources/game/DiscreteFlow.d");
  gRegMod("./sources/game/Location.d");
  gRegMod("./sources/game/Mission.d");
  gRegMod("./sources/game/Module.d");
  gRegMod("./sources/game/Port.d");
  gRegMod("./sources/game/Prototype.d");
  gRegMod("./sources/game/Station.d");
  gRegMod("./sources/guilib/Button.d");
  gRegMod("./sources/guilib/Dimension.d");
  gRegMod("./sources/guilib/Font.d");
  gRegMod("./sources/guilib/GUIManager.d");
  gRegMod("./sources/guilib/Point.d");
  gRegMod("./sources/guilib/Props.d");
  gRegMod("./sources/guilib/Rect.d");
  gRegMod("./sources/layers/CameraControl.d");
  gRegMod("./sources/layers/ModuleLayer.d");
  gRegMod("./sources/layers/SkyLayer.d");
  gRegMod("./sources/main/Main.d");
  gRegMod("./sources/main/Platform.d");
  gRegMod("./sources/main/Timer.d");
  gRegMod("./sources/math/Angle.d");
  gRegMod("./sources/math/AxisAlignedBox.d");
  gRegMod("./sources/math/Math.d");
  gRegMod("./sources/math/Matrix.d");
  gRegMod("./sources/math/Quaternion.d");
  gRegMod("./sources/math/Ray.d");
  gRegMod("./sources/math/Vector.d");
  gRegMod("./sources/rendering/Camera.d");
  gRegMod("./sources/rendering/JP2Coder.d");
  gRegMod("./sources/rendering/KDTree.d");
  gRegMod("./sources/rendering/LoadImage.d");
  gRegMod("./sources/rendering/Mesh.d");
  gRegMod("./sources/rendering/RenderSystem.d");
  gRegMod("./sources/rendering/Surface.d");
  gRegMod("./sources/rendering/Texture.d");
  gRegMod("./sources/scheme/Eval.d");
  gRegMod("./sources/scheme/Library.d");
  gRegMod("./sources/scheme/Read.d");
  gRegMod("./sources/scheme/Value.d");
  gRegMod("./sources/screens/Filters.d");
  gRegMod("./sources/screens/HUD.d");
  gRegMod("./sources/screens/Layers.d");
  gRegMod("./sources/screens/ModuleDock.d");
  gRegMod("./sources/screens/ShellScreen.d");
  gRegMod("./sources/screens/StationScreen.d");
  gRegMod("./sources/screens/ToolScreen.d");
  gRegMod("./sources/util/Check.d");
  gRegMod("./sources/util/Log.d");
  gRegMod("./sources/util/TestFixture.d");
  gRegMod("./sources/Version.d");
  gRegMod("./sources/win32/EntryWin32.d");
  gRegMod("./sources/win32/PlatformWin32.d");  
}


// --------------------------------------------------------------------------------

private char[] gErrorMessage = "Undefined";

extern (C) void _d_assert(char[] filename, uint line)
{
  char[] message = gErrorMessage;
  gErrorMessage = "Undefined error";
  throw new Exception(gFileFromModule[filename] ~ "(" ~ std.string.toString(line) ~ "): " ~ message);
}


// --------------------------------------------------------------------------------
// << file << "(" << std.string.toString(line) << "): " 

bool
CHECK_FAIL(char[] message)
{
  gErrorMessage = message;
  return false;
}

bool
CHECK(char[] message, int test)
{
  if(!test) {
    gErrorMessage = message;
    return false;
  }
  return true;
}

bool
CHECK_EQUAL(char[] message, char[] expected, char[] actual)
{
  if(cmp(expected, actual) != 0) {
    gErrorMessage = message ~ " Expected: \"" ~ expected ~ "\" Actual: \"" ~ actual ~ "\"";
    return false;
  }
  return true;
}

bool
CHECK_EQUAL(char[] message, int expected, int actual)
{
  if(expected != actual) {
    gErrorMessage = message ~ " Expected: " ~ std.string.toString(expected) ~
      " Actual: " ~ std.string.toString(actual);
    return false;
  }
  return true;
}

bool
CHECK_EQUAL(char[] message, uint expected, uint actual)
{
  if(expected != actual) {
    gErrorMessage = message ~ " Expected: " ~ std.string.toString(expected) ~
      " Actual: " ~ std.string.toString(actual);
    return false;
  }
  return true;
}

bool
CHECK_EQUAL(char[] message, float expected, float actual)
{
  if(expected != actual) {
    gErrorMessage = message ~ " Expected: " ~ std.string.toString(expected) ~
      " Actual: " ~ std.string.toString(actual);
    return false;
  }
  return true;
}

