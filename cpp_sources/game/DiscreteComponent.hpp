//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef DISCRETE_COMPONENT_HPP
#define DISCRETE_COMPONENT_HPP

class Module;
class DiscreteComponent;

const float INFINITE_TIME_ADVANCE = FLT_MAX;
const float STANDARD_SPEED_FACTOR = 2;
const float STANDARD_QUANTA = 0.2;

typedef vector<DiscreteComponent*> DiscreteHeap;
void percolateDESHeap(DiscreteHeap& desHeap, unsigned int index);

class DiscreteComponent
{
public:
  DiscreteComponent(Module* module, int index, float quanta = STANDARD_QUANTA, float speedfactor = STANDARD_SPEED_FACTOR);

  void updateSimulation(float discreteTime);
  void updateTimeAdvance(float discreteTime);
  void resetTimeAdvance(float oldDiscreteTime);
  void zeroTimeAdvance();

  float getNextUpdate() {return mNextUpdate;}

  int getIndex() {return mIndex;}
  void setIndex(int index) {mIndex = index;}  
  
  float getQuanta() {return mQuanta;}
  void setQuanta(float quanta) {mQuanta = quanta;}

  void setTargetLevel(float level) {mTargetLevel = level; mCurrentLevel = level; }
  float getTargetLevel() {return mTargetLevel;}
  float getCurrentLevel() {return mCurrentLevel;}

  virtual float findTimeAdvance(float ) {return 0;}

  Module* mModule;
  int mIndex;

  float mQuanta;
  float mSpeedFactor;

  float mCurrentLevel;
  float mTargetLevel;

  float mLastUpdate;
  float mNextUpdate;
  unsigned int mHeapIndex;
};


#endif
