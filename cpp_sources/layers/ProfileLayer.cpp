//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "ProfileLayer.hpp"

#include "guilib/FlowLayout.hpp"
#include "guilib/Frame.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/Label.hpp"
#include "main/Main.hpp"
#include "rendering/RenderSystem.hpp"

#include "profiler/prof.h"
#include "profiler/prof_internal.h"

// --------------------------------------------------------------------------------
// Singleton implementation

ProfileLayer*
rProfileLayer()
{
  static ProfileLayer* mProfileLayer = new ProfileLayer();
  return mProfileLayer;
}

// --------------------------------------------------------------------------------

ProfileLayer::ProfileLayer()
  : mProfileViewer(NULL), mProfileData(NULL), mFPSTrend(-1), mTriTrend(-1)
{
  GridLayout* layout = new GridLayout(this, 32, 24);
  mProfileViewer = new Frame();
  layout->add(mProfileViewer, 24, 0, 8, 16);
  mProfileData = new Label(L"Profile data");
  mProfileViewer->add(mProfileData);
  setVisible(false);
}

ProfileLayer::~ProfileLayer()
{
  delete mProfileViewer;
  delete mProfileData;
}


// --------------------------------------------------------------------------------


void
ProfileLayer::update(float)
{
  float frameTime = rMain()->getFrameTime();
  int frameTriangles = rRenderSystem()->getFrameTriangles();

  float FPS;
  if(frameTime != 0)
    FPS = 1 / frameTime;
  else
    FPS = 1;

  float tri = FPS * frameTriangles * 0.001f;
  if(mFPSTrend == -1)
    mFPSTrend = FPS;
  else
    mFPSTrend = 0.95 * mFPSTrend + 0.05 * FPS;
  if(mTriTrend == -1)
    mTriTrend = tri;
  else
    mTriTrend = 0.95 * mTriTrend + 0.05 * tri;

  wostringstream wossStatus;
  wossStatus << "FPS: " << fixed << setprecision(1) << mFPSTrend << endl
             << "Tri: " << fixed << setprecision(2) << mTriTrend << "k/s" << endl << endl;

  // print profile data
  {
    Prof_Report* report = Prof_create_report();
    //    wossStatus << "Profile title: " << report->title[0] << " " << report->title[1] << endl;
    for(int iHeader = 0; iHeader < NUM_HEADER; ++iHeader) {
      if(report->header[iHeader])
        wossStatus << report->header[iHeader] << " ";
    }
    wossStatus << endl;
    for(int iRecord = 0; iRecord < report->num_record; ++iRecord) {
      Prof_Report_Record& record = report->record[iRecord];
      for(int indent = 0; indent < record.indent; ++indent)
        wossStatus << " ";
      if(record.number)
        wossStatus << record.name << " (" << record.number << ") ";
      else
        wossStatus << record.name << " ";
      for (int iValue = 0; iValue < NUM_VALUES; ++iValue) {
        if(record.value_flag & (1 << iValue)) {
          wossStatus << setprecision(1) << fixed << record.values[iValue] << " ";
        }
      }
      wossStatus << endl;
      
    }
    
    Prof_free_report(report);

  }
  

  mProfileData->setLabel(wossStatus.str());
}

