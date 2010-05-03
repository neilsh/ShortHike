//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_QUATERNION_PROPERTY
#define PROPERTY_QUATERNION_PROPERTY

class QuaternionProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  QuaternionProperty(wstring displayName, string memberName, ulong offset)
    : Property(displayName, memberName, offset)
  {
  }

  Quaternion& getQuaternion(Item* target)
  {
    Quaternion* data = static_cast<Quaternion*>(getDataPointer(target));
    return *data;
  }

  void setQuaternion(Item* target, const Quaternion& newValue)
  {
    Quaternion* data = static_cast<Quaternion*>(getDataPointer(target));
    *data = newValue;
  }

  virtual bool compare(Item* item1, Item* item2) 
  {
    if(Property::compare(item1, item2) == false) return false;
    return getQuaternion(item1) == getQuaternion(item2);
  }  

  virtual void copy(Item* source, Item* destination, ItemDictionary )
  {
    setQuaternion(destination, getQuaternion(source));
  }  
};


#endif
