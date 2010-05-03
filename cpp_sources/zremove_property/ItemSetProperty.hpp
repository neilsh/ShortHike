//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_ITEM_SET_PROPERTY
#define PROPERTY_ITEM_SET_PROPERTY

class ItemSetProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  ItemSetProperty(wstring displayName, string memberName, ulong offset, string _className)
    : Property(displayName, memberName, offset), className(_className)
  {
  }

  string getClassName()
  {
    return className;
  }

  set<Item*>& getItems(Item* target)
  {
    set<Item*>* data = static_cast<set<Item*>*>(getDataPointer(target));
    return *data;
  }

  void setItems(Item* target, const set<Item*>& newValue)
  {
    for(set<Item*>::const_iterator itemI = newValue.begin(); itemI != newValue.end(); ++itemI) {
      Item* currentItem = *itemI;
      CHECK("New item has correct class", currentItem->getClassName() == getClassName());
    }
    
    set<Item*>* data = static_cast<set<Item*>*>(getDataPointer(target));
    *data = newValue;
    firePropertyChanged(target);
  }  

  virtual bool isPointerType() const
  {
    return true;
  }

  virtual bool hasItemPointers(Item* target) const
  {
    set<Item*>* data = static_cast<set<Item*>*>(getDataPointer(target));
    return data->size() != 0;
  }

  virtual vector<Item*> getItemPointers(Item* target)
  {
    vector<Item*> items;
    if(hasItemPointers(target) == true) {
      set<Item*>* data = static_cast<set<Item*>*>(getDataPointer(target));
      items.insert(items.begin(), data->begin(), data->end());
    }    
    return items;
  }


  virtual bool compare(Item* item1, Item* item2) 
  {
    if(Property::compare(item1, item2) == false) return false;
    return true;
  }


  virtual void copy(Item* source, Item* destination, ItemDictionary dictionary)
  {
    set<Item*>& sourceItems = getItems(source);
    set<Item*>& targetItems = getItems(destination);
    for(set<Item*>::const_iterator itemI = sourceItems.begin(); itemI != sourceItems.end(); ++itemI) {
      Item* currentItem = *itemI;
      targetItems.insert(dictionary[currentItem]);
    }
  }

private:
  string className;
};


#endif
