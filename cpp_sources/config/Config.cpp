//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "Config.hpp"
#include "Player.hpp"
#include "drm/License.hpp"
#include "property/Property.hpp"

//--------------------------------------------------------------------------------

START_PROPERTY_DEFINITION(Config)
{
  addProperty(new BoolProperty(L"This is set at the start of an update and cleared when the update is complete.", SetMember(mUpdateInProgress)));

  addProperty(new BoolProperty(L"Setting this to true shows the loading items in the loading dialog", SetMember(showLoadingItems)));
  addProperty(new RealVectorProperty(L"Vector of loading ticks saved from previous run", SetMember(loadTicks)));

  addProperty(new IntProperty(L"The number of the display monitor to show ShortHike on, starting from 0", SetMember(displayMonitor)));
  addProperty(new IntProperty(L"AntiAliasing setting", SetMember(antiAliasing)));

  addProperty(new ItemProperty(L"License information", SetMember(mLicense), License::getStaticClassName()));

  addProperty(new BoolProperty(L"Should we use the complete distribution", SetMember(mCompleteDistribution)));
}
END_PROPERTY_DEFINITION

//--------------------------------------------------------------------------------

Config::Config(DataFile* dataFile)
  : Item(dataFile),
    showLoadingItems(false),
    displayMonitor(0), antiAliasing(0), mDisableShaders(false),
    mUpdateInProgress(false), mLicense(NULL), mCompleteDistribution(false)
{
}


