//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------
// The property system is usually included as a single chunk through this file

#ifndef PROPERTY_STRING_PROPERTY
#define PROPERTY_STRING_PROPERTY

#pragma warning(disable: 4100)

class StringProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  StringProperty(wstring displayName, string memberName, ulong offset)
    : Property(displayName, memberName, offset)
  {
  }

  string& getString(Item* target)
  {
    string* data = static_cast<string*>(getDataPointer(target));
    return *data;
  }

  void setString(Item* target, const string& newValue)
  {
    string* data = static_cast<string*>(getDataPointer(target));
    *data = newValue;
    firePropertyChanged(target);
  }

  virtual bool compare(Item* item1, Item* item2) 
  {
    if(Property::compare(item1, item2) == false) return false;
    return getString(item1) == getString(item2);
  }

  virtual void copy(Item* source, Item* destination, ItemDictionary)
  {
    setString(destination, getString(source));
  }  
};


#endif
