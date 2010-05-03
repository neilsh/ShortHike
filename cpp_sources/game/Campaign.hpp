//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------
// Campaign settings are not store with player profiles directly. Instead the profile
// stores only the complete missions and the campaign calculates status on the fly.

#pragma once
#ifndef GAME_CAMPAIGN_HPP
#define GAME_CAMPAIGN_HPP

#include "file/Persistent.hpp"
class ChunkIterator;

class Campaign : public Persistent
{
public:
  Campaign() 
  {
  }
  
  Campaign(string handle, wstring name, int order);
  ~Campaign();
  
  int getOrder() const {return mOrder;}
  const wstring& getName() const {return mName;}

  void addGroup(vector<string> missions);
  bool hasMission(string missionHandle);
  
  virtual bool load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr);
  virtual bool save(ChunkStream* chunkStream);
private:
  string mHandle;
  wstring mName;
  int mOrder;
  
  set<string> mMissions;
  vector<vector<string> > mGroups;
};


#endif
