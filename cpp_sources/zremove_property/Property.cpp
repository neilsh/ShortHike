//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------
// The property system is usually included as a single chunk through this file


#include "Common.hpp"

#include "Property.hpp"
#include "Item.hpp"

//--------------------------------------------------------------------------------
// Type declarations

// NEW PROPERTY: Add correct property type
string StringProperty::propertyType = "StringProperty";
string WStringProperty::propertyType = "WStringProperty";
string BoolProperty::propertyType = "BoolProperty";
string IntProperty::propertyType = "IntProperty";
string RealProperty::propertyType = "RealProperty";
string RealVectorProperty::propertyType = "RealVectorProperty";
string ItemProperty::propertyType = "ItemProperty";
string ItemVectorProperty::propertyType = "ItemVectorProperty";
string ItemSetProperty::propertyType = "ItemSetProperty";
string Vector3Property::propertyType = "Vector3Property";
string QuaternionProperty::propertyType = "QuaternionProperty";

//--------------------------------------------------------------------------------

Property::Property(wstring _displayName, string _memberName, ulong _offset)
  : displayName(_displayName), memberName(_memberName), offset(_offset)
{
}

const string&
Property::getMemberName() const
{
  return memberName;
}


//--------------------------------------------------------------------------------

bool
Property::compare(Item* item1, Item* item2)
{
  if(item1 == item2) return true;

  Property* property2 = item2->getProperty(getMemberName());
  if(property2 == NULL) return false;

  if(getPropertyType() != property2->getPropertyType()) return false;
  
  return true;
}

//--------------------------------------------------------------------------------
void
Property::firePropertyChanged(Item* target) const
{
  target->propertyChanged(memberName);
}  


//================================================================================
// Unit tests

TEST_FIXTURE_DEFINITION("unit/property/TestProperty", TestProperty);

#include "TestData.hpp"

//--------------------------------------------------------------------------------

void
TestProperty::string_property()
{
  SampleItem* testItem = new SampleItem();
  StringProperty* property = dynamic_cast<StringProperty*>(testItem->getProperty("myTestString"));
  CHECK("Property casted properly", property != NULL);
  testItem->myTestString = "Test1";
  
  CHECK_EQUAL("Property reading matches", string, string("Test1"), property->getString(testItem));  

  property->setString(testItem, "Test2");
  CHECK_EQUAL("Direct reading matches", string, "Test2", testItem->myTestString);
  CHECK_EQUAL("Property reading matches", string, "Test2", property->getString(testItem));

  CHECK("No pointers", property->hasItemPointers(testItem) == false);

  Item::destroyItem(testItem);
}

//--------------------------------------------------------------------------------

void
TestProperty::real_property()
{
  SampleItem* testItem = new SampleItem();
  RealProperty* property = dynamic_cast<RealProperty*>(testItem->getProperty("myTestReal"));
  CHECK("Property casted properly", property != NULL);
  testItem->myTestReal = 100.0;
  
  CHECK_EQUAL("Property reading matches", Real, 100.0, property->getReal(testItem));  

  property->setReal(testItem, 500.0);
  CHECK_EQUAL("Direct reading matches", Real, 500.0, testItem->myTestReal);
  CHECK_EQUAL("Property reading matches", Real, 500.0, property->getReal(testItem));

  CHECK("No pointers", property->hasItemPointers(testItem) == false);

  Item::destroyItem(testItem);
}


//--------------------------------------------------------------------------------

void
TestProperty::item_property()
{
  SampleItem* testItem1 = new SampleItem();
  testItem1->myTestItem = new SampleItem();

  ItemProperty* property = dynamic_cast<ItemProperty*>(testItem1->getProperty("myTestItem"));
  CHECK("Property casted properly", property != NULL);

  Item* childItem = property->getItem(testItem1);
  CHECK("Child item is valid", childItem != NULL);
  SampleItem* testItem2 = dynamic_cast<SampleItem*>(childItem);
  CHECK("Child item is casted property", testItem2 != NULL);  

  testItem1->myTestItem->myTestString = "Test5";
  CHECK_EQUAL("Child reference works", string, "Test5", testItem2->myTestString);

  CHECK("Has pointers", property->hasItemPointers(testItem1));
  CHECK("Has a single pointer", property->getItemPointers(testItem1).size() == 1);
  CHECK("No pointers", property->hasItemPointers(testItem2) == false);

  Item::destroyItem(testItem1);
}

//--------------------------------------------------------------------------------

void
TestProperty::item_vector_property()
{
  SampleItem* testItem1 = new SampleItem();
  SampleItem* testItem2 = new SampleItem();
  SampleItem* testItem3 = new SampleItem();

  testItem1->myItems.push_back(testItem2);
  testItem1->myItems.push_back(testItem3);

  ItemVectorProperty* property = dynamic_cast<ItemVectorProperty*>(testItem1->getProperty("myItems"));
  CHECK("Property casted properly", property != NULL);

  CHECK("Has pointers", property->hasItemPointers(testItem1));
  CHECK("Has a two pointers", property->getItemPointers(testItem1).size() == 2);  

  CHECK("Has pointers", testItem1->getProperty("myItems")->hasItemPointers(testItem1));
  CHECK("Has a two pointers", testItem1->getProperty("myItems")->getItemPointers(testItem1).size() == 2);  

  Item::destroyItem(testItem1);
}

//--------------------------------------------------------------------------------

void
TestProperty::item_set_property()
{
  SampleItem* testItem1 = new SampleItem();
  SampleItem* testItem2 = new SampleItem();
  SampleItem* testItem3 = new SampleItem();

  testItem1->myItemSet.insert(testItem2);
  testItem1->myItemSet.insert(testItem3);

  ItemSetProperty* property = dynamic_cast<ItemSetProperty*>(testItem1->getProperty("myItemSet"));
  CHECK("Property casted properly", property != NULL);

  CHECK("Has pointers", property->hasItemPointers(testItem1));
  CHECK("Has a two pointers", property->getItemPointers(testItem1).size() == 2);  

  CHECK("Has pointers", testItem1->getProperty("myItemSet")->hasItemPointers(testItem1));
  CHECK("Has a two pointers", testItem1->getProperty("myItemSet")->getItemPointers(testItem1).size() == 2);  

  Item::destroyItem(testItem1);
}


void
TestProperty::real_vector_property()
{
  SampleItem* testItem1 = new SampleItem();
  testItem1->myTestRealVector.push_back(1.0);
  testItem1->myTestRealVector.push_back(2.0);
  Item* testItem2 = testItem1->clone(NULL);
  CHECK("Real vectors copied correctly", compare(testItem1, testItem2));
  testItem1->myTestRealVector.push_back(3.0);
  CHECK("Real vectors compared correctly", compare(testItem1, testItem2) == false);
}


//--------------------------------------------------------------------------------


void
TestProperty::comparison()
{
  SampleItem* testItem1 = new SampleItem();
  SampleItem* testItem2 = new SampleItem();
  SampleItem* testItem3 = new SampleItem();

  testItem1->myTestItem = testItem3;
  testItem2->myTestItem = testItem3;

  testItem3->myTestString = "Different string";
  testItem3->myTestReal = 5.0;
  
  Property* stringProperty = testItem1->getProperty("myTestString");
  CHECK("testItem1 is equal to itself", stringProperty->compare(testItem1, testItem1) == true);
  CHECK("testItem1 is equal to testItem2", stringProperty->compare(testItem1, testItem2) == true);
  CHECK("testItem1 is not equal to testItem3", stringProperty->compare(testItem1, testItem3) == false);

  Property* realProperty = testItem1->getProperty("myTestReal");
  CHECK("testItem1 is equal to itself", realProperty->compare(testItem1, testItem1) == true);
  CHECK("testItem1 is equal to testItem2", realProperty->compare(testItem1, testItem2) == true);
  CHECK("testItem1 is not equal to testItem3", realProperty->compare(testItem1, testItem3) == false);

  Property* itemProperty = testItem1->getProperty("myTestItem");
  CHECK("testItem1 is equal to itself", itemProperty->compare(testItem1, testItem1) == true);
  CHECK("testItem1 is equal to testItem2", itemProperty->compare(testItem1, testItem2) == true);
  CHECK("testItem1 is not equal to testItem3", itemProperty->compare(testItem1, testItem3) == false);
}
