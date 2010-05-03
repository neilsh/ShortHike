//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_BOOL_PROPERTY
#define PROPERTY_BOOL_PROPERTY

class BoolProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  BoolProperty(wstring displayName, string memberName, ulong offset)
    : Property(displayName, memberName, offset)
  {
  }

  bool& getBool(Item* target)
  {
    bool* data = static_cast<bool*>(getDataPointer(target));
    return *data;
  }

  void setBool(Item* target, const bool& newValue)
  {
    bool* data = static_cast<bool*>(getDataPointer(target));
    *data = newValue;
    firePropertyChanged(target);
  }

  virtual bool compare(Item* item1, Item* item2) 
  {
    if(Property::compare(item1, item2) == false) return false;
    return getBool(item1) == getBool(item2);
  }  

  virtual void copy(Item* source, Item* destination, ItemDictionary)
  {
    setBool(destination, getBool(source));
  }  
};


#endif
