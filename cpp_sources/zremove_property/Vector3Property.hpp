//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_VECTOR3_PROPERTY
#define PROPERTY_VECTOR3_PROPERTY

class Vector3Property : public Property
{
  DEFINE_NEW_PROPERTY
public:
  Vector3Property(wstring displayName, string memberName, ulong offset)
    : Property(displayName, memberName, offset)
  {
  }

  Vector3& getVector3(Item* target)
  {
    Vector3* data = static_cast<Vector3*>(getDataPointer(target));
    return *data;
  }

  void setVector3(Item* target, const Vector3& newValue)
  {
    Vector3* data = static_cast<Vector3*>(getDataPointer(target));
    *data = newValue;
  }

  virtual bool compare(Item* item1, Item* item2) 
  {
    if(Property::compare(item1, item2) == false) return false;
    return getVector3(item1) == getVector3(item2);
  }  

  virtual void copy(Item* source, Item* destination, ItemDictionary)
  {
    setVector3(destination, getVector3(source));
  }  
};


#endif
