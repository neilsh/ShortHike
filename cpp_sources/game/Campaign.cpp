//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "Campaign.hpp"

#include "file/ChunkData.hpp"
#include "file/ChunkStream.hpp"

//--------------------------------------------------------------------------------


Campaign::Campaign(string handle, wstring name, int order)
  : mHandle(handle), mName(name), mOrder(order)
{
}

Campaign::~Campaign()
{
}

// --------------------------------------------------------------------------------

bool
Campaign::load(const ChunkIterator& , const ChunkIterator& , ChunkIterator& )
{
  return true;
}

bool
Campaign::save(ChunkStream* )
{
  return true;
}



// --------------------------------------------------------------------------------
void
Campaign::addGroup(vector<string> missionGroup)
{
  mGroups.push_back(missionGroup);
  for(vector<string>::const_iterator iMission = missionGroup.begin(); iMission != missionGroup.end(); ++iMission) {
    string missionHandle = *iMission;
    mMissions.insert(missionHandle);
  }  
}

bool
Campaign::hasMission(string missionHandle)
{
  return mMissions.find(missionHandle) != mMissions.end();
}
