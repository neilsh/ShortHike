//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_ITEM_VECTOR_PROPERTY
#define PROPERTY_ITEM_VECTOR_PROPERTY

class ItemVectorProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  ItemVectorProperty(wstring displayName, string memberName, ulong offset, string _className)
    : Property(displayName, memberName, offset), className(_className)
  {
  }

  string getClassName()
  {
    return className;
  }

  vector<Item*>& getItems(Item* target)
  {
    vector<Item*>* data = static_cast<vector<Item*>*>(getDataPointer(target));
    return *data;
  }

  void setItems(Item* target, const vector<Item*>& newValue)
  {
    for(vector<Item*>::const_iterator itemI = newValue.begin(); itemI != newValue.end(); ++itemI) {
      Item* currentItem = *itemI;
      CHECK("New item has correct class", currentItem->getClassName() == getClassName());
    }

    vector<Item*>* data = static_cast<vector<Item*>*>(getDataPointer(target));
    *data = newValue;
    firePropertyChanged(target);
  }  

  virtual bool isPointerType() const
  {
    return true;
  }

  virtual bool hasItemPointers(Item* target) const
  {
    vector<Item*>* data = static_cast<vector<Item*>*>(getDataPointer(target));
    return data->size() != 0;
  }

  virtual vector<Item*> getItemPointers(Item* target)
  {
    vector<Item*> items;
    if(hasItemPointers(target) == true) {
      vector<Item*>* data = static_cast<vector<Item*>*>(getDataPointer(target));
      items.insert(items.begin(), data->begin(), data->end());
    }    
    return items;
  }

  virtual bool doPointersMatch() const
  {
    return true;
  }

  virtual void copy(Item* source, Item* destination, ItemDictionary dictionary)
  {
    vector<Item*>& sourceItems = getItems(source);
    vector<Item*>& targetItems = getItems(destination);
    for(vector<Item*>::const_iterator itemI = sourceItems.begin(); itemI != sourceItems.end(); ++itemI) {
      Item* currentItem = *itemI;
      targetItems.push_back(dictionary[currentItem]);
    }
  }  

private:
  string className;
};


#endif
