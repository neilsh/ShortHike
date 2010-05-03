//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------
// The property system is usually included as a single chunk through this file

#ifndef PROPERTY_WSTRING_PROPERTY
#define PROPERTY_WSTRING_PROPERTY

#pragma warning(disable: 4100)

class WStringProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  WStringProperty(wstring displayName, string memberName, ulong offset)
    : Property(displayName, memberName, offset)
  {
  }

  wstring& getString(Item* target)
  {
    wstring* data = static_cast<wstring*>(getDataPointer(target));
    return *data;
  }

  void setString(Item* target, const wstring& newValue)
  {
    wstring* data = static_cast<wstring*>(getDataPointer(target));
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
