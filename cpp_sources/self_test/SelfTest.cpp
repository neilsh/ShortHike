//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "SelfTest.hpp"
#include "rendering/RenderSystem.hpp"

RenderSystem*
createRenderSystem()
{
  return NULL;
}


INT WINAPI WinMain( HINSTANCE , HINSTANCE, LPSTR arguments, INT )
{
  string prefix(arguments);
  if(prefix == "") prefix = "unit";

//   TestFixture::run_test_fixtures(prefix);
  TestFixture::run_test_fixtures("unit/file");
  
  return 0;
}
