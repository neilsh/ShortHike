//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "PrototypeManager.hpp"
#include "Property.hpp"

//--------------------------------------------------------------------------------
// Singleton implementation

template<> PrototypeManager*
Singleton<PrototypeManager>::ms_Singleton = 0;

//--------------------------------------------------------------------------------

PrototypeManager::PrototypeManager()
  : serial(0)
{
}

//--------------------------------------------------------------------------------

void
PrototypeManager::addPrototype(string ident, Item* prototype)
{
  if(prototypes[ident] != NULL) 
    Item::destroyItem(prototypes[ident]);

  prototypes[ident] = prototype;
  prototype->setIdentifier(ident);
}

void
PrototypeManager::removePrototype(Item* prototype)
{
  string ident = prototype->getIdentifier();
  CHECK("Has valid identifier", ident != "" && getPrototype(ident) != NULL);
  prototypes.erase(ident);
}


//--------------------------------------------------------------------------------

Item*
PrototypeManager::getPrototype(string ident)
{
  if(prototypes.find(ident) != prototypes.end())
    return prototypes[ident];
  else
    return NULL;  
}


//--------------------------------------------------------------------------------

Item*
PrototypeManager::createInstance(DataFile* dataFile, string ident)
{
  Item* prototype = getPrototype(ident);
  if(prototype == NULL) {
    return NULL;
  }
  else {
    Item* item = static_cast<Item*>(prototype->createInstance(dataFile));
    return item;
  }  
}

//--------------------------------------------------------------------------------

Item*
PrototypeManager::createClone(DataFile* dataFile, string ident)
{
  Item* prototype = getPrototype(ident);
  if(prototype == NULL) {
    return NULL;
  }
  else {
    Item* item = static_cast<Item*>(prototype->clone(dataFile));
    item->setPrototypeName(prototype->getIdentifier());
    return item;
  }  
}


//--------------------------------------------------------------------------------

const PrototypeManager::PrototypeList&
PrototypeManager::getAllPrototypes()
{
  return prototypes;
}



//--------------------------------------------------------------------------------

string
PrototypeManager::getNextId()
{
  ostringstream itemName;
  itemName << serial;
  serial++;
  return itemName.str();
}


//================================================================================
// Unit tests

TEST_FIXTURE_DEFINITION("unit/property/TestPrototypeManager", TestPrototypeManager);

#include "TestData.hpp"


//--------------------------------------------------------------------------------

void
TestPrototypeManager::prototype()
{
  PrototypeManager* pManager = PrototypeManager::getSingletonPtr();
  
  SampleItem* prototype = createSampleItem();
  pManager->addPrototype("prototype", prototype);
  SampleItem* instance = pManager->createInstanceClass<SampleItem>(NULL, "prototype");
  CHECK("Instance is not NULL", instance != NULL);
  CHECK("Instance is equal to prototype", compare(prototype, instance));
  CHECK_EQUAL("Instance has proper prototype name", string, "prototype", instance->getPrototypeName());

  // Check for how changes are registered
  instance->myTestReal = 500.0;
  instance->myTestBool = !prototype->myTestBool;
  instance->myTestString = "Instance has changed";
  CHECK_EQUAL("One property is changed", unsigned int, 3, instance->getDifferingProperties().size());
  instance->myTestReal = prototype->myTestReal;
  instance->myTestBool = prototype->myTestBool;
  instance->myTestString = prototype->myTestString;
  CHECK_EQUAL("No properties are changed", unsigned int, 0, instance->getDifferingProperties().size());  

  Item::destroyItem(prototype);
  CHECK("Prototype is removed from PrototypeManager", pManager->getPrototype("prototype") == NULL);
  Item::destroyItem(instance);
}
