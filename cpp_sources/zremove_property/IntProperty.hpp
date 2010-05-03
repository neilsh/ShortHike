//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_INT_PROPERTY
#define PROPERTY_INT_PROPERTY

class IntProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  IntProperty(wstring displayName, string memberName, ulong offset)
    : Property(displayName, memberName, offset)
  {
  }

  int& getInt(Item* target)
  {
    int* data = static_cast<int*>(getDataPointer(target));
    return *data;
  }

  void setInt(Item* target, const int& newValue)
  {
    int* data = static_cast<int*>(getDataPointer(target));
    *data = newValue;
    firePropertyChanged(target);
  }

  virtual bool compare(Item* item1, Item* item2) 
  {
    if(Property::compare(item1, item2) == false) return false;
    return getInt(item1) == getInt(item2);
  }  

  virtual void copy(Item* source, Item* destination, ItemDictionary)
  {
    setInt(destination, getInt(source));
  }  
};


#endif
