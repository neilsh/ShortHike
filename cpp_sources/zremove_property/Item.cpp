//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "Item.hpp"
#include "Property.hpp"
#include "DataFile.hpp"
#include "PrototypeManager.hpp"


//--------------------------------------------------------------------------------

Item::Item(DataFile* _dataFile)
  : dataFile(_dataFile)
{
  setDataFile(dataFile);
}

Item::~Item()
{
  setDataFile(NULL);
}



//--------------------------------------------------------------------------------

map<string, ItemCreatorFunction>&
Item::creatorFunctions()
{
  static map<string, ItemCreatorFunction>* _creatorFunctions = new map<string, ItemCreatorFunction>();
  return *_creatorFunctions;
}

//--------------------------------------------------------------------------------

Item*
Item::createItem(string className, DataFile* dataFile)
{
  ItemCreatorFunction creator = creatorFunctions()[className];
  if(creator != NULL)
    return creator(dataFile);
  else 
    return NULL;
}

void
Item::registerItemClass(string className, ItemCreatorFunction creatorFunction)
{
  creatorFunctions()[className] = creatorFunction;
}



//--------------------------------------------------------------------------------

template<class ItemClass>
ItemClass*
ItemcastToClass()
{
  if(this->getClasName() != ItemClass::getStaticClassName()) {
    return NULL;
  }
  else {
    return static_cast<ItemClass*>(this);
  }
}


//--------------------------------------------------------------------------------


Property*
Item::getProperty(string memberName)
{
  PropertyMap& props = getProperties();
  return props[memberName];
}

void
Item::addPropertyImpl(PropertyMap& props, Property* newProperty)
{
  string memberName = newProperty->getMemberName();
  props[memberName] = newProperty;
  addMemberName(memberName);
}

//--------------------------------------------------------------------------------
// Use this item and its childern as a prototype and create a new instance

Item*
Item::createInstance(DataFile* _newDataFile)
{
  DataFile* newDataFile = _newDataFile;
  if(newDataFile == NULL)
    newDataFile = getDataFile();
  
  map<Item*, Item*> originalToCopy;
  set<Item*> allItems = findConnectedItems();
  
  for(set<Item*>::const_iterator itemI = allItems.begin(); itemI != allItems.end(); ++itemI) {
    Item* originalItem = *itemI;
    Item* copyItem = Item::createItem(originalItem->getClassName(), newDataFile);
    originalToCopy[originalItem] = copyItem;    
    copyItem->setPrototypeName(originalItem->getIdentifier());
  }

  // Now when we have the dictionary filled we are ready to do the real copying
  for(set<Item*>::const_iterator itemI = allItems.begin(); itemI != allItems.end(); ++itemI) {
    Item* original = *itemI;
    Item* copy = originalToCopy[original];

    PropertyMap properties = original->getProperties();
    for(PropertyMap::const_iterator propertyI = properties.begin(); propertyI != properties.end(); ++propertyI) {
      Property* property = (*propertyI).second;
      property->copy(original, copy, originalToCopy);
    }
    copy->copyFrom(original);
  }  

  return originalToCopy[this];
}

//--------------------------------------------------------------------------------
// A pure clone method. Doesn *not* copy pointer members.

Item*
Item::clone(DataFile* _newDataFile)
{  
  DataFile* newDataFile = _newDataFile;
  if(newDataFile == NULL)
    newDataFile = getDataFile();

  Item* copyItem = Item::createItem(getClassName(), newDataFile);
  map<Item*, Item*> originalToCopy;

  PropertyMap properties = getProperties();
  for(PropertyMap::const_iterator propertyI = properties.begin(); propertyI != properties.end(); ++propertyI) {
    Property* property = (*propertyI).second;
    if(property->isPointerType() == false)
      property->copy(this, copyItem, originalToCopy);
    copyItem->copyFrom(this);
  }

  return copyItem;
}


//--------------------------------------------------------------------------------

void
Item::destroyItem(Item* target)
{
  set<Item*> items = target->findConnectedItems();
  for(set<Item*>::const_iterator itemI = items.begin(); itemI != items.end(); ++itemI) {
    Item* currentItem = *itemI;
    if(currentItem->getIdentifier() != "")
      PrototypeManager::getSingleton().removePrototype(currentItem);
    delete currentItem;
  }  
}


//--------------------------------------------------------------------------------

set<Item*>
Item::findConnectedItems()
{
  // Loop through all items and create initial copy of them and populate dictionary
  set<Item*> foundItems;
  queue<Item*> toScan;

  toScan.push(this);
  foundItems.insert(this);
  while(!toScan.empty()) {
    Item* originalItem = toScan.front();
    toScan.pop();

    PropertyMap properties = originalItem->getProperties();
    for(PropertyMap::const_iterator propertyI = properties.begin(); propertyI != properties.end(); ++propertyI) {
      Property* property = (*propertyI).second;
      if(property->hasItemPointers(originalItem)) {
        vector<Item*> pointers = property->getItemPointers(originalItem);
        for(unsigned int itemI = 0; itemI < pointers.size(); ++itemI) {
          Item* target = pointers[itemI];
          if(foundItems.find(target) == foundItems.end()) {
            toScan.push(target);
            foundItems.insert(target);
          }
        }
      }
    }
  }
  return foundItems;
}


//--------------------------------------------------------------------------------
// Deep comparing of two datasets

bool
compare(Item* item1, Item* item2, bool verbose)
{
  if(item1 == NULL && item2 == NULL) return true;
  if(item1 == NULL || item2 == NULL) return false;

  set<Item*> processed;
  queue<pair<Item*, Item*> > toProcess;
  
  toProcess.push(pair<Item*, Item*>(item1, item2));

  while(!toProcess.empty()) {
    Item* first = toProcess.front().first;
    Item* second = toProcess.front().second;
    if(verbose)
      cout << "Item: className: " << first->getClassName() << " - " << second->getClassName() << endl;
    if(first->getClassName() != second->getClassName()) return false;

    PropertyMap properties = first->getProperties();
    for(PropertyMap::const_iterator propertyI = properties.begin(); propertyI != properties.end(); ++propertyI) {
      Property* property = (*propertyI).second;
      if(verbose)
        cout << " Property: " << property->getMemberName() << endl;
      if(property->hasItemPointers(first) != property->hasItemPointers(second)) return false;

      // Push unprocessed items on queue
      if(property->doPointersMatch() && property->hasItemPointers(first)) {
        vector<Item*> pointers1 = property->getItemPointers(first);
        vector<Item*> pointers2 = property->getItemPointers(second);
        if(pointers1.size() != pointers2.size()) return false;
        for(unsigned int itemI = 0; itemI < pointers1.size(); ++itemI) {
          Item* target1 = pointers1[itemI];
          Item* target2 = pointers2[itemI];
          if(processed.find(target1) == processed.end()) {
            toProcess.push(pair<Item*, Item*>(target1, target2));            
          }
        }
      }
      // Check property equality
      else {
        if(verbose)
          cout << " Property: Comparing .." << endl;
        if(property->compare(first, second) == false) return false;
        if(verbose)
          cout << " Property: Compare done .." << endl;
      }      
    }
    
    if(verbose)
      cout << " Property: Done processing .." << endl;
    processed.insert(first);
    toProcess.pop();
  }

  return true;
}



//--------------------------------------------------------------------------------

DataFile*
Item::getDataFile()
{
  return dataFile;
}

void
Item::setDataFile(DataFile* _dataFile)
{
  if(dataFile != NULL)
    dataFile->unRegisterItem(this);
  
  dataFile = _dataFile;

  if(dataFile != NULL)
    dataFile->registerItem(this);
}

//--------------------------------------------------------------------------------
// When the Item is a prototype itself the identifier is used to keep track
// of its prototype name

const string&
Item::getIdentifier() const
{
  return identifier;
}

void
Item::setIdentifier(string _identifier)
{
  identifier = _identifier;
}


//--------------------------------------------------------------------------------

const string&
Item::getPrototypeName() const
{
  return prototypeName;
}


void
Item::setPrototypeName(string _prototypeName)
{
  prototypeName = _prototypeName;
}


PropertyMap
Item::getDifferingProperties()
{
  PropertyMap differences;
  Item* prototype = PrototypeManager::getSingleton().getPrototype(getPrototypeName());
  if(getPrototypeName() == "" || prototype == NULL) {
    differences = getProperties();
    return differences;
  }
  CHECK("Compared against prototype of same class", prototype->getClassName() == getClassName());

  PropertyMap properties = getProperties();
  for(PropertyMap::const_iterator propertyI = properties.begin(); propertyI != properties.end(); ++propertyI) {
    Property* property = (*propertyI).second;

    // Add pointers if they contain anything of interest
    if(property->isPointerType() == true) {
      if(property->hasItemPointers(this))
        differences.insert(*propertyI);
    }
    // Atomic properties are compared before addition
    else if(property->compare(prototype, this) == false) {
      differences.insert(*propertyI);
    }
  }
  
  return differences;
}

//--------------------------------------------------------------------------------
// The member name dictionary is a global string to int mapper that is used by
// DB backends to optimize storage. As properties are compiled in we can deduce
// this statically at startup time and it thus doesn't change during the
// execution of the simulator. DB's store a local version as the mapping might
// have changed since last compilation.
// These use static variables to get around the startup dilemma.

void
Item::addMemberName(string memberName)
{
  static int counter = 100;
  map<string, int>& dictionary = getMemberNameDictionary();
  if(dictionary.find(memberName) == dictionary.end()) {
    dictionary[memberName] = counter++;
  }
}

map<string, int>&
Item::getMemberNameDictionary()
{
  static map<string, int> dictionary;
  return dictionary;
}



//================================================================================
// Unit tests

TEST_FIXTURE_DEFINITION("unit/property/TestItem", TestItem);

#include "TestData.hpp"

void
TestItem::item_creation()
{
  Item* testItem = Item::createItem("SampleItem", NULL);
  string testItemClass = testItem->getClassName();

  SampleItem* testSampleItem = static_cast<SampleItem*>(testItem);
  CHECK("Sample items are property casted", testSampleItem != NULL);  
}


void
TestItem::deep_compare()
{
  SampleItem* sampleItem1 = createSampleTree();
  SampleItem* sampleItem2 = createSampleTree();
  SampleItem* sampleItem3 = createSampleTree();

  sampleItem3->myTestItem->myTestString = "Not Equal";

  CHECK("Tree1 is equal to itself", compare(sampleItem1, sampleItem1) == true);
  CHECK("Tree1 is equal to Tree2", compare(sampleItem1, sampleItem2) == true);
  CHECK("Tree1 is not equal to Tree3", compare(sampleItem1, sampleItem3) == false);
}

void
TestItem::find_connected()
{
  SampleItem* sampleItem1 = createSampleTree();
  CHECK_EQUAL("Correct number of items", unsigned int, 4, sampleItem1->findConnectedItems().size());
  Item::destroyItem(sampleItem1);
}

#include "game/Module.hpp"

void
TestItem::create_instance()
{
  SampleItem* sampleItem1 = createSampleTree();
  Item* sampleItem2 = sampleItem1->createInstance(NULL);

  CHECK("Clone is equal to original", compare(sampleItem1, sampleItem2));
}


