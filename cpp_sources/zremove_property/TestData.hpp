//--------------------------------------------------------------------------------
// Test functions shared by several unit tests

namespace 
{
  string name("testString");
  
  //--------------------------------------------------------------------------------

  class SampleItem : public Item
  {
    PROPERTY_DECLARATION(SampleItem);
  public:
    SampleItem(DataFile* dataFile = NULL)
      : Item(dataFile), myTestItem(NULL)
    {
    }    

    virtual void propertyChanged(string )
    {
    }

    string myTestString;
    wstring myTestWString;
    bool myTestBool;
    int myTestInt;
    Real myTestReal;
    SampleItem* myTestItem;
    vector<SampleItem*> myItems;
    set<SampleItem*> myItemSet;
    Vector3 myVector3;
    Quaternion myQuaternion;
    vector<Real> myTestRealVector;
  private:
    ~SampleItem() {}    
  };

  START_PROPERTY_DEFINITION(SampleItem)
  {
    addProperty(new StringProperty(L"Test string property",
                                   SetMember(myTestString)));
    addProperty(new WStringProperty(L"Test wstring property",
                                   SetMember(myTestWString)));
    addProperty(new BoolProperty(L"Test bool property", SetMember(myTestBool)));
    addProperty(new IntProperty(L"Test int property", SetMember(myTestInt)));
    addProperty(new RealProperty(L"Test Real property",
                                   SetMember(myTestReal)));
    addProperty(new RealVectorProperty(L"Test RealVector property", SetMember(myTestRealVector)));
    addProperty(new ItemProperty(L"Test Item property",
                                   SetMember(myTestItem), SampleItem::getStaticClassName()));
    addProperty(new ItemVectorProperty(L"Test vector of Items",
                                       SetMember(myItems), SampleItem::getStaticClassName()));
    addProperty(new ItemSetProperty(L"Test set of Items",
                                       SetMember(myItemSet), SampleItem::getStaticClassName()));
    addProperty(new Vector3Property(L"Test Vector2",
                                    SetMember(myVector3)));
    addProperty(new QuaternionProperty(L"Test Quaternion",
                                       SetMember(myQuaternion)));
  }
  END_PROPERTY_DEFINITION


  //--------------------------------------------------------------------------------

  SampleItem*
  createSampleItem(DataFile* dataFile = NULL)
  {
    SampleItem* item1 = new SampleItem(dataFile);

    item1->myTestString = "Item1";
    item1->myTestWString = L"Item1 wstring";
    item1->myTestBool = true;
    item1->myTestReal = 100.5;
    item1->myTestRealVector.push_back(1.0f);
    item1->myTestRealVector.push_back(1.1f);
    item1->myVector3 = Vector3(1, 2, 3);
    item1->myQuaternion = Quaternion(1, 2, 3, 4);

    return item1;
  }


  SampleItem*
  createSampleTree(DataFile* dataFile = NULL)
  {
    SampleItem* item1 = new SampleItem(dataFile);
    SampleItem* item2 = new SampleItem(dataFile);
    SampleItem* item3 = new SampleItem(dataFile);
    SampleItem* item4 = new SampleItem(dataFile);

    item1->myTestItem = item2;
    item2->myItems.push_back(item3);
    item2->myItems.push_back(item4);

    item2->myItemSet.insert(item3);
    item2->myItemSet.insert(item2);
    item2->myItemSet.insert(item1);
    
    item1->myTestString = "Item1";
    item2->myTestString = "Item2";
    item3->myTestString = "Item3";
    item4->myTestString = "Item4";

    item1->myTestWString = L"Item1 wstring";
    item2->myTestWString = L"Item2 wstring";
    item3->myTestWString = L"Item3 wstring";
    item4->myTestWString = L"Item4 wstring";

    item1->myTestBool = true;
    item2->myTestBool = false;
    item3->myTestBool = true;
    item4->myTestBool = false;

    item1->myTestInt = 10;
    item2->myTestInt = 20;
    item3->myTestInt = 30;
    item4->myTestInt = 40;

    item1->myTestReal = 100.5;
    item2->myTestReal = 200.5;
    item3->myTestReal = 300.5;
    item4->myTestReal = 400.5;

    item1->myTestRealVector.push_back(1.0f);
    item2->myTestRealVector.push_back(2.0f);
    item3->myTestRealVector.push_back(3.0f);
    item4->myTestRealVector.push_back(4.0f);
    item1->myTestRealVector.push_back(1.1f);
    item2->myTestRealVector.push_back(2.1f);
    item3->myTestRealVector.push_back(3.1f);
    item4->myTestRealVector.push_back(4.1f);

    item4->myTestItem = item1;

    item1->myVector3 = Vector3(1, 2, 3);
    item2->myVector3 = Vector3(4, 5, 6);
    item3->myVector3 = Vector3(7, 8, 9);
    item4->myVector3 = Vector3(10, 11, 12);
    
    item1->myQuaternion = Quaternion(1, 2, 3, 4);
    item2->myQuaternion = Quaternion(5, 6, 7, 8);
    item3->myQuaternion = Quaternion(9, 10, 11, 12);
    item4->myQuaternion = Quaternion(13, 14, 15, 16);

    return item1;
  }  
}
