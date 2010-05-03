//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Module;

const float INFINITE_TIME_ADVANCE = float.max;
const float STANDARD_SPEED_FACTOR = 2;
const float STANDARD_QUANTA = 0.2;

class DiscreteComponent
{

  //--------------------------------------------------------------------------------

  this(Module _module, int index, float quanta = STANDARD_QUANTA, float speedfactor = STANDARD_SPEED_FACTOR)
  {
    mModule = _module;
    mIndex = index;
    mQuanta = quanta;
    mSpeedFactor = speedfactor;
    mCurrentLevel = 0;
    mTargetLevel = 0;
    mLastUpdate = 0;
    mNextUpdate = 0;
  }

  // --------------------------------------------------------------------------------

  void
  updateSimulation(float discreteTime)
  {
    float timeElapsed = discreteTime - mLastUpdate;  
    float difference = mTargetLevel - mCurrentLevel;
    float velocity = mSpeedFactor * difference;
    float delta = velocity * timeElapsed;
    if(fabs(delta) > fabs(difference))
      mCurrentLevel = mTargetLevel;
    else
      mCurrentLevel += delta;
    mLastUpdate = discreteTime;  

    updateTimeAdvance(discreteTime);
  
    Module[] ports = mModule.getConnections();
    for(int iPort = 0; iPort < ports.length; ++iPort) {
      if(ports[iPort] !is null) {
        ports[iPort].getDiscreteComponent(mIndex).updateTimeAdvance(discreteTime);
      }    
    }
  
    // REFACTOR remove
    //   const PortList& ports = mModule.getPorts();
    //   for(PortIterator portI = ports.begin(); portI != ports.end(); ++portI) {
    //     Port* currentPort = *portI;
    //     if(currentPort.isConnected()) {
    //       Module* otherModule = currentPort.getConnection().getHost();
    //       otherModule.getDiscreteComponent(mIndex).updateTimeAdvance(discreteTime);
    //     }
    //   }
  }


  void
  updateTimeAdvance(float discreteTime)
  {
    float timeAdvance = findTimeAdvance(discreteTime);
    float timeElapsed = discreteTime - mLastUpdate;
    if(INFINITE_TIME_ADVANCE == timeAdvance)
      mNextUpdate = INFINITE_TIME_ADVANCE;
    else
      mNextUpdate = discreteTime + timeAdvance - timeElapsed;
    if(mNextUpdate < discreteTime)
      mNextUpdate = discreteTime;

    if(mModule.getStation() !is null)
      percolateDESHeap(mModule.getStation().getDiscreteHeap(), mHeapIndex);
  }


  // --------------------------------------------------------------------------------

  void
  resetTimeAdvance(float oldDiscreteTime)
  {
    if(INFINITE_TIME_ADVANCE != mNextUpdate) {
      mNextUpdate -= oldDiscreteTime;
      mLastUpdate -= oldDiscreteTime;
    }
  }

  void
  zeroTimeAdvance()
  {
    mLastUpdate = 0;
    mNextUpdate = 0;
  }

public:


  float getNextUpdate() {return mNextUpdate;}

  int getIndex() {return mIndex;}
  void setIndex(int index) {mIndex = index;}  
  
  float getQuanta() {return mQuanta;}
  void setQuanta(float quanta) {mQuanta = quanta;}

  void setTargetLevel(float level) {mTargetLevel = level; mCurrentLevel = level; }
  float getTargetLevel() {return mTargetLevel;}
  float getCurrentLevel() {return mCurrentLevel;}

  float findTimeAdvance(float time) {return 0;}

  Module mModule;
  int mIndex;

  float mQuanta;
  float mSpeedFactor;

  float mCurrentLevel;
  float mTargetLevel;

  float mLastUpdate;
  float mNextUpdate;
  int mHeapIndex;
}

// --------------------------------------------------------------------------------

void
swapDESHeap(DiscreteComponent[] desHeap, int index1, int index2)
{
  DiscreteComponent first = desHeap[index1];
  DiscreteComponent second = desHeap[index2];

  desHeap[index1] = second;
  desHeap[index2] = first;

  second.mHeapIndex = index1;
  first.mHeapIndex = index2;
}


void
percolateDESHeap(DiscreteComponent[] desHeap, int index)
{
  int heapSize = desHeap.length;
  if(index > heapSize) return;

  DiscreteComponent pegComponent = desHeap[index];
  float pegUpdate = pegComponent.mNextUpdate;

  int leftChild = index * 2 + 1;
  int rightChild = index * 2 + 2;
  while(rightChild < heapSize &&
        (desHeap[rightChild].mNextUpdate < pegUpdate || desHeap[leftChild].mNextUpdate < pegUpdate)) {
    if(desHeap[leftChild].mNextUpdate < desHeap[rightChild].mNextUpdate) {
      swapDESHeap(desHeap, index, leftChild);
      index = leftChild;
    }
    else {
      swapDESHeap(desHeap, index, rightChild);
      index = rightChild;
    }
    leftChild = index * 2 + 1;
    rightChild = index * 2 + 2;    
  }
  if(leftChild < heapSize && desHeap[leftChild].mNextUpdate < pegUpdate) {
    swapDESHeap(desHeap, index, leftChild);
    index = leftChild;
  }  
        
  int parent = (index - 1) / 2;
  while(index > 0 && desHeap[parent].mNextUpdate > pegUpdate) {
    swapDESHeap(desHeap, index, parent);
    index = parent;
    parent = (index - 1) / 2;
  }
}

void
printDESHeap(DiscreteComponent[] desHeap)
{
  for(int iComponent = 0; iComponent < desHeap.length; ++iComponent) {
    logInfo << iComponent << ": " << desHeap[iComponent].mHeapIndex << endl;
  }  
}

