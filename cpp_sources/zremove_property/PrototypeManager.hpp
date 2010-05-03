//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef PROPERTY_PROTOTYPE_MANAGER_HPP
#define PROPERTY_PROTOTYPE_MANAGER_HPP

class Item;
class DataFile;

class PrototypeManager : public Singleton<PrototypeManager>
{
public:
  PrototypeManager();

  typedef map<string, Item*> PrototypeList;

  void addPrototype(string ident, Item* prototype);
  Item* getPrototype(string ident);
  Item* createInstance(DataFile* dataFile, string ident);
  Item* createClone(DataFile* dataFile, string ident);
  void removePrototype(Item* prototype);

  const PrototypeList& getAllPrototypes();

  string getNextId();

  //--------------------------------------------------------------------------------
  // Convenience methods for easier access to specific classes of prototypes

  template<class ItemClass> ItemClass* getPrototypeClass(string ident)
  {
    return castItemToClass<ItemClass>(getPrototype(ident));
  }
  
  template<class ItemClass> ItemClass* createInstanceClass(DataFile* dataFile, string ident)
  {
    return castItemToClass<ItemClass>(createInstance(dataFile, ident));
  }

  template<class ItemClass> ItemClass* createCloneClass(DataFile* dataFile, string ident)
  {
    return castItemToClass<ItemClass>(createClone(dataFile, ident));
  }

  template<class ItemClass> map<string, ItemClass*> getPrototypes()
  {
    map<string, ItemClass*> targets;
    for(PrototypeList::iterator itemI = prototypes.begin(); itemI != prototypes.end(); ++itemI) {
      ItemClass* item = castItemToClass<ItemClass>((*itemI).second);
      if(item == NULL) continue;
      string ident = (*itemI).first;
      targets.insert(pair<string, ItemClass*>(ident, item));
    }
    return targets;
  }

    
  template<class ItemClass> static ItemClass* castItemToClass(Item* item)
  {
    if(item == NULL) return NULL;
    
    if(ItemClass::getStaticClassName() != item->getClassName())
      return NULL;

    return static_cast<ItemClass*>(item);
  }  
  
private:
  PrototypeList prototypes;

  int serial;
};


//--------------------------------------------------------------------------------

class TestPrototypeManager : public TestFixture
{
public:
  TEST_FIXTURE_START(TestPrototypeManager);
  TEST_CASE(prototype);
  TEST_FIXTURE_END();

  void prototype();
};


#endif
