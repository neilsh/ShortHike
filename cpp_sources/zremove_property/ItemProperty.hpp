//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_ITEM_PROPERTY
#define PROPERTY_ITEM_PROPERTY

#include "Item.hpp"

class ItemProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  ItemProperty(wstring displayName, string memberName, ulong offset, string _className)
    : Property(displayName, memberName, offset), className(_className)
  {
  }

  Item* getItem(Item* target)
  {
    Item** data = static_cast<Item**>(getDataPointer(target));
    return *data;
  }

  void setItem(Item* target, Item* newValue)
  {
    if(newValue != NULL)
      CHECK("Item has valid class", newValue->getClassName() == className);
    
    Item** data = static_cast<Item**>(getDataPointer(target));
    *data = newValue;
    firePropertyChanged(target);
  }

  virtual bool isPointerType() const
  {
    return true;
  }  

  virtual bool hasItemPointers(Item* target) const
  {
    Item** data = static_cast<Item**>(getDataPointer(target));
    return *data != NULL;
  }  

  virtual vector<Item*> getItemPointers(Item* target)
  {
    vector<Item*> items;
    if(hasItemPointers(target) == true) {
      Item** data = static_cast<Item**>(getDataPointer(target));
      items.push_back(*data);
    }    
    return items;
  }

  string getClassName()
  {
    return className;
  }


  virtual bool compare(Item* item1, Item* item2) 
  {
    if(Property::compare(item1, item2) == false) return false;
    return getItem(item1) == getItem(item2);
  }

  virtual bool doPointersMatch() const
  {
    return true;
  }


  virtual void copy(Item* source, Item* destination, ItemDictionary dictionary)
  {
    setItem(destination, dictionary[getItem(source)]);
  }  

private:
  string className;
};


#endif
