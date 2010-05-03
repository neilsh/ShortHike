//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef PROPERTY_REAL_VECTOR_PROPERTY
#define PROPERTY_REAL_VECTOR_PROPERTY

class RealVectorProperty : public Property
{
  DEFINE_NEW_PROPERTY
public:
  RealVectorProperty(wstring displayName, string memberName, ulong offset)
    : Property(displayName, memberName, offset)
  {
  }

  vector<Real>& getRealVector(Item* target)
  {
    vector<Real>* data = static_cast<vector<Real>*>(getDataPointer(target));
    return *data;
  }

  void setRealVector(Item* target, const vector<Real>& newValue)
  {
    vector<Real>* data = static_cast<vector<Real>*>(getDataPointer(target));
    *data = newValue;
    firePropertyChanged(target);
  }

  virtual bool compare(Item* item1, Item* item2) 
  {
    const MAGIC_FLOAT_COMPARE_GRANULE = 0.001f;
    if(Property::compare(item1, item2) == false) return false;
    vector<Real>& vector1 = getRealVector(item1);
    vector<Real>& vector2 = getRealVector(item2);
    if(vector1.size() != vector2.size()) return false;
    for(unsigned int numberI = 0; numberI < vector1.size(); ++numberI) {
#pragma warning(disable: 4189)
      float num1 = vector1[numberI];
      float num2 = vector2[numberI];
      if(abs(vector1[numberI] - vector2[numberI]) > MAGIC_FLOAT_COMPARE_GRANULE) return false;
    }
    return true;
  }  

  virtual void copy(Item* source, Item* destination, ItemDictionary dictionary)
  {
    setRealVector(destination, getRealVector(source));
    CHECK("Real vectors copied properly", compare(source, destination) == true);
  }  
};


#endif
