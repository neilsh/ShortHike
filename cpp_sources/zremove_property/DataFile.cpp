//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------
// When adding a new property check all the places tagged NEW PROPERTY and add
// as required. Also add your property to the test Items in TestData.hpp

#include "Common.hpp"

#include "DataFile.hpp"
#include "Item.hpp"
#include "Property.hpp"
#include "PrototypeManager.hpp"
#include "generated/build_info.hpp"
#include "SQLiteImpl1.hpp"

//--------------------------------------------------------------------------------

DataFile::DataFile(wstring _fileName, bool _silentErrors)
  : fileName(_fileName), dataFileImpl(NULL), rootItem(NULL), silentErrors(_silentErrors)
{
  dataFileImpl = new SQLiteImpl1(this);
  if(fileName != L"") {
    dataFileImpl->load();
    for(set<Item*>::const_iterator itemI = items.begin(); itemI != items.end(); ++itemI) {
      Item* currentItem = *itemI;
      currentItem->postLoad();
    }
  }  
}

DataFile::~DataFile()
{  
  if(rootItem != NULL)
    Item::destroyItem(rootItem);
  if(items.size() != 0) {
    for(set<Item*>::const_iterator itemI = items.begin(); itemI != items.end(); ++itemI) {
      Item* currentItem = *itemI;
      ostringstream itemInfo;
      itemInfo << "WARNING: Item not connected to root: " << currentItem->getClassName();
      logEngineWarn(itemInfo.str());
    }
  }
#ifdef BUILD_UNSTABLE
  // This is disabled in stable builds as we haven't yet been able to locate the bug
  //  CHECK_EQUAL("RootItem is connected to all items", int, 0, items.size());
#endif
}

//--------------------------------------------------------------------------------

wstring
DataFile::getFileName()
{
  return fileName;
}

//--------------------------------------------------------------------------------

void
DataFile::save(wstring _fileName)
{
  fileName = _fileName;
  save();
}


void
DataFile::save()
{
  CHECK("Filename is valid", fileName != L"");
  CHECK("Root item is not NULL", rootItem != NULL);

  for(set<Item*>::const_iterator itemI = items.begin(); itemI != items.end(); ++itemI) {
    Item* currentItem = *itemI;
    currentItem->preSave();
  }

  dataFileImpl->save();
}


//--------------------------------------------------------------------------------

Item*
DataFile::getRootItem()
{
  return rootItem;
}


void
DataFile::setRootItem(Item* _rootItem)
{
  rootItem = _rootItem;
}

void
DataFile::registerItem(Item* target)
{
  items.insert(target);
}

void
DataFile::unRegisterItem(Item* target)
{
  items.erase(target);
  if(rootItem == target)
    rootItem = NULL;
}


//--------------------------------------------------------------------------------

bool
DataFile::isPartOfFile(Item* item)
{
  return items.find(item) != items.end();
}




//================================================================================
// Unit tests

TEST_FIXTURE_DEFINITION("unit/property/DataFile", TestDataFile);

#include "TestData.hpp"

//--------------------------------------------------------------------------------

void runPrototypeTest(string prototypeName, wstring dbName, int itemNumber, bool verbose = false)
{
  SampleItem* prototype = createSampleItem(NULL);
  PrototypeManager* pManager = PrototypeManager::getSingletonPtr();
  pManager->addPrototype(prototypeName, prototype);

  DataFile* dataFile = new DataFile();
  SampleItem* rootSampleItem = pManager->createCloneClass<SampleItem>(dataFile, prototypeName);
  dataFile->setRootItem(rootSampleItem);
  
  for(int itemI = 0; itemI < itemNumber; ++itemI) {
    SampleItem* child = pManager->createCloneClass<SampleItem>(dataFile, prototypeName);
    if((itemI % 10) == 0)
      child->myTestString = "Different variation";
    rootSampleItem->myItems.push_back(child);
  }

  DWORD start = GetTickCount();
  if(verbose)
    cout << "Prototype SAVE: Start" << endl;
  dataFile->save(dbName);
  if(verbose)
    cout << "Prototype SAVE: End " << (static_cast<float>((GetTickCount() - start)) / 1000.0) << "s" << endl;

  start = GetTickCount();
  if(verbose)
    cout << "Prototype LOAD: Start" << endl;
  DataFile* dataFile2 = new DataFile(dbName);
  if(verbose)
    cout << "Prototype LOAD: End " << (static_cast<float>((GetTickCount() - start)) / 1000.0) << "s" << endl;

  CHECK("Saved and loaded are equal", compare(dataFile->getRootItem(), dataFile2->getRootItem()) == true);

  delete dataFile;  
  delete dataFile2;
  pManager->removePrototype(prototype);
}


//--------------------------------------------------------------------------------


void
TestDataFile::save_load()
{
  DataFile* dataFile1 = new DataFile();
  SampleItem* item1 = createSampleTree(dataFile1);
  dataFile1->setRootItem(item1);
  dataFile1->save(L"testdata/TestDataFile_save_load.shd");

  DataFile* dataFile2 = new DataFile(L"testdata/TestDataFile_save_load.shd");
  SampleItem* item2 = static_cast<SampleItem*>(dataFile2->getRootItem());
  
  CHECK("Loaded Item tree is equal to original", compare(item1, item2));

  item1->myTestString = "Changed";
  item2->myTestString = "Changed";

  dataFile2->save();

  DataFile* dataFile3 = new DataFile(L"testdata/TestDataFile_save_load.shd");
  Item* item3 = dataFile3->getRootItem();

  CHECK("Resaved Item tree is equal to original", compare(item1, item3));  

  delete dataFile1;
  delete dataFile2;
  delete dataFile3;
}

void
TestDataFile::save_modify()
{
  DataFile* dataFile1 = new DataFile();
  SampleItem* item1 = createSampleTree(dataFile1);
  dataFile1->setRootItem(item1);
  dataFile1->save(L"testdata/TestDataFile_save_modify.shd");

  DataFile* dataFile2 = new DataFile(L"testdata/TestDataFile_save_modify.shd");
  SampleItem* item2 = static_cast<SampleItem*>(dataFile2->getRootItem());
  
  SampleItem* newItem = new SampleItem(dataFile2);
  item2->myTestItem->myTestItem = newItem;
  newItem->myTestItem = item2;
  dataFile2->save();

  DataFile* dataFile3 = new DataFile(L"testdata/TestDataFile_save_modify.shd");

  delete dataFile1;
  delete dataFile2;
  delete dataFile3;

}


//--------------------------------------------------------------------------------
// Test that items properly unregister on delete. This used to be a crash problem.

void
TestDataFile::item_destroy()
{
  DataFile* dataFile = new DataFile();

  SampleItem* item1 = new SampleItem(dataFile);
  SampleItem* item2 = new SampleItem(dataFile);
  item1->myTestItem = item2;
  dataFile->setRootItem(item1);
  Item::destroyItem(item1);
  CHECK("Item1 is not part of file", dataFile->isPartOfFile(item1) == false);
  CHECK("Item2 is not part of file", dataFile->isPartOfFile(item2) == false);
  CHECK("Root item is NULL", dataFile->getRootItem() == NULL);

  delete dataFile;
}

//--------------------------------------------------------------------------------
// Cloning had a problem with littering the DataFile with extra Items

void
TestDataFile::item_clone()
{
  DataFile* dataFile = new DataFile();

  SampleItem* item1 = createSampleTree(dataFile);
  int numberOfItems = dataFile->getNumberOfItems();
  Item* item2 = item1->createInstance(dataFile);
  CHECK_EQUAL("The number of items has doubled", int, numberOfItems*2, dataFile->getNumberOfItems());
  Item* item3 = item1->createInstance(dataFile);
  CHECK_EQUAL("The number of items has tripled", int, numberOfItems*3, dataFile->getNumberOfItems());

  Item::destroyItem(item1);
  Item::destroyItem(item2);
  Item::destroyItem(item3);

  delete dataFile;
}

//--------------------------------------------------------------------------------
// Prototype save and load

void
TestDataFile::prototype_save_load()
{
  runPrototypeTest("prototype_save_load", L"testdata/TestDataFile_prototype_save_load.shd", 50);
}

//--------------------------------------------------------------------------------
// Load a corrupted databases

void
TestDataFile::corrupted_database()
{
  DataFile* dataFile = new DataFile(L"testData/TestDataFile/corrupted_database.shd", true);
  CHECK("Root is NULL", dataFile->getRootItem() == NULL);
  delete dataFile;
}



//================================================================================
// Stress testing of datafile performance. The purpose of this stress test is
// to find potential bottlenecks in the saving performance of DataFiles

TEST_FIXTURE_DEFINITION("stress/property/DataFile", TestDataFileStress);

void
TestDataFileStress::save_stress()
{
  DataFile* dataFile = new DataFile();

  SampleItem* rootSampleItem = new SampleItem(dataFile);
  dataFile->setRootItem(rootSampleItem);
  const int ITEM_NUMBER = 3000;

  for(int itemI = 0; itemI < ITEM_NUMBER; ++itemI) {
    SampleItem* child = createSampleTree(dataFile);
    rootSampleItem->myItems.push_back(child);
  }
  
  DWORD start = GetTickCount();
  cout << "SAVE: Start" << endl;
  dataFile->save(L"testdata/TestDataFileStress_save_stress.shd");
  cout << "SAVE: End " << (static_cast<float>((GetTickCount() - start)) / 1000.0) << "s" << endl;
  delete dataFile;

  start = GetTickCount();
  cout << "LOAD: Start" << endl;
  dataFile = new DataFile(L"testdata/TestDataFileStress_save_stress.shd");
  cout << "LOAD: End " << (static_cast<float>((GetTickCount() - start)) / 1000.0) << "s" << endl;
  delete dataFile;
}


//--------------------------------------------------------------------------------
//
// Using the prototype base saving: 2.4MB
// FileSize: 2.4MB
// Prototype SAVE: Start
// Prototype SAVE: End 13.468s
// Prototype LOAD: Start
// Prototype LOAD: End 29.469s
//
// Without prototypes:
// FileSize: 9.8MB
// Prototype SAVE: Start
// Prototype SAVE: End 63.61s
// Prototype LOAD: Start
// Prototype LOAD: End 44.171s


void
TestDataFileStress::prototype_save_stress()
{
  runPrototypeTest("prototype_save_stress", L"testdata/TestDataFileStress_prototype_save_stress.shd", 20000, true);
}
