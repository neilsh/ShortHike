//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "DiscreteComponent.hpp"
#include "Station.hpp"
#include "Module.hpp"
#include "Port.hpp"

//--------------------------------------------------------------------------------

DiscreteComponent::DiscreteComponent(Module* module, int index, float quanta, float speedFactor)
  : mModule(module), mIndex(index),
    mQuanta(quanta), mSpeedFactor(speedFactor), mCurrentLevel(0), mTargetLevel(0),
    mLastUpdate(0), mNextUpdate(0)
{
}

// --------------------------------------------------------------------------------

void
DiscreteComponent::updateSimulation(float discreteTime)
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
  
  const vector<Module*>& ports = mModule->getConnections();
  for(unsigned int iPort = 0; iPort < ports.size(); ++iPort) {
    if(ports[iPort] != NULL) {
      ports[iPort]->getDiscreteComponent(mIndex)->updateTimeAdvance(discreteTime);
    }    
  }
  
  // REFACTOR remove
//   const PortList& ports = mModule->getPorts();
//   for(PortIterator portI = ports.begin(); portI != ports.end(); ++portI) {
//     Port* currentPort = *portI;
//     if(currentPort->isConnected()) {
//       Module* otherModule = currentPort->getConnection()->getHost();
//       otherModule->getDiscreteComponent(mIndex)->updateTimeAdvance(discreteTime);
//     }
//   }
}


void
DiscreteComponent::updateTimeAdvance(float discreteTime)
{
  float timeAdvance = findTimeAdvance(discreteTime);
  float timeElapsed = discreteTime - mLastUpdate;
  if(INFINITE_TIME_ADVANCE == timeAdvance)
    mNextUpdate = INFINITE_TIME_ADVANCE;
  else
    mNextUpdate = discreteTime + timeAdvance - timeElapsed;
  if(mNextUpdate < discreteTime)
    mNextUpdate = discreteTime;

  if(mModule->getStation() != NULL)
    percolateDESHeap(mModule->getStation()->getDiscreteHeap(), mHeapIndex);
}


// --------------------------------------------------------------------------------

void
DiscreteComponent::resetTimeAdvance(float oldDiscreteTime)
{
  if(INFINITE_TIME_ADVANCE != mNextUpdate) {
    mNextUpdate -= oldDiscreteTime;
    mLastUpdate -= oldDiscreteTime;
  }
}

void
DiscreteComponent::zeroTimeAdvance()
{
  mLastUpdate = 0;
  mNextUpdate = 0;
}

// --------------------------------------------------------------------------------

inline void
swapDESHeap(DiscreteHeap& desHeap, unsigned int index1, unsigned int index2)
{
  DiscreteComponent* first = desHeap[index1];
  DiscreteComponent* second = desHeap[index2];

  desHeap[index1] = second;
  desHeap[index2] = first;

  second->mHeapIndex = index1;
  first->mHeapIndex = index2;
}


void
percolateDESHeap(DiscreteHeap& desHeap, unsigned int index)
{
  unsigned int heapSize = desHeap.size();
  if(index > heapSize) return;

  DiscreteComponent* pegComponent = desHeap[index];
  float pegUpdate = pegComponent->mNextUpdate;

  unsigned int leftChild = index * 2 + 1;
  unsigned int rightChild = index * 2 + 2;
  while(rightChild < heapSize &&
        (desHeap[rightChild]->mNextUpdate < pegUpdate || desHeap[leftChild]->mNextUpdate < pegUpdate)) {
    if(desHeap[leftChild]->mNextUpdate < desHeap[rightChild]->mNextUpdate) {
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
  if(leftChild < heapSize && desHeap[leftChild]->mNextUpdate < pegUpdate) {
    swapDESHeap(desHeap, index, leftChild);
    index = leftChild;
  }  
        
  unsigned int parent = (index - 1) / 2;
  while(index > 0 && desHeap[parent]->mNextUpdate > pegUpdate) {
    swapDESHeap(desHeap, index, parent);
    index = parent;
    parent = (index - 1) / 2;
  }
}


