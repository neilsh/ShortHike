//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------
// The property system is usually included as a single chunk through this file

#ifndef PROPERTY_PROPERTY_HPP
#define PROPERTY_PROPERTY_HPP

class Item;

typedef map<Item*, Item*>& ItemDictionary;

class Property
{
public:
  Property(wstring displayName, string memberName, ulong offset);

  const string& getMemberName() const;
  const wstring& getDisplayName() const {return displayName;}

  virtual bool isPointerType() const {return false;}
  virtual bool hasItemPointers(Item* ) const {return false;};
  virtual vector<Item*> getItemPointers(Item* ) {return vector<Item*>();}  
  virtual bool doPointersMatch() const {return false;}

  virtual bool compare(Item* item1, Item* item2);
  virtual void copy(Item* original, Item* copy, ItemDictionary dictionary) = 0;

  virtual const string& getPropertyType() const = 0;

protected:
  void* getDataPointer(Item* base) const
  {
    return reinterpret_cast<void*>(reinterpret_cast<ulong>(base) + offset);
  }

  void firePropertyChanged(Item* target) const;

  wstring displayName;
  string memberName;
  
  ulong offset;
};

//--------------------------------------------------------------------------------

#define DEFINE_NEW_PROPERTY                                         \
                                                                    \
public:                                                             \
  virtual const string& getPropertyType() const                     \
  {                                                                 \
    return propertyType;                                            \
  }                                                                 \
                                                                    \
private:                                                            \
  static string propertyType;



//--------------------------------------------------------------------------------
// Include different property types that are all defined in hpp files

// NEW PROPERTY: Add include here
#include "StringProperty.hpp"
#include "WStringProperty.hpp"
#include "RealProperty.hpp"
#include "IntProperty.hpp"
#include "RealVectorProperty.hpp"
#include "BoolProperty.hpp"
#include "ItemProperty.hpp"
#include "ItemVectorProperty.hpp"
#include "ItemSetProperty.hpp"
#include "Vector3Property.hpp"
#include "QuaternionProperty.hpp"

//--------------------------------------------------------------------------------

class TestProperty : public TestFixture
{
public:
  TEST_FIXTURE_START(TestProperty);
  TEST_CASE(string_property);
  TEST_CASE(real_property);
  TEST_CASE(item_property);
  TEST_CASE(item_vector_property);
  TEST_CASE(item_set_property);
  TEST_CASE(real_vector_property);
  TEST_CASE(comparison);
  TEST_FIXTURE_END();

  void string_property();
  void real_property();
  void item_property();
  void item_vector_property();
  void item_set_property();
  void real_vector_property();
  void comparison();
private:
};


#endif
