//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_REAL_PROPERTY
#define PROPERTY_REAL_PROPERTY

class RealProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  RealProperty(wstring displayName, string memberName, ulong offset)
    : Property(displayName, memberName, offset)
  {
  }

  Real& getReal(Item* target)
  {
    Real* data = static_cast<Real*>(getDataPointer(target));
    return *data;
  }

  void setReal(Item* target, const Real& newValue)
  {
    Real* data = static_cast<Real*>(getDataPointer(target));
    *data = newValue;
    firePropertyChanged(target);
  }

  virtual bool compare(Item* item1, Item* item2) 
  {
    if(Property::compare(item1, item2) == false) return false;
    return getReal(item1) == getReal(item2);
  }  

  virtual void copy(Item* source, Item* destination, ItemDictionary)
  {
    setReal(destination, getReal(source));
  }  
};


#endif
