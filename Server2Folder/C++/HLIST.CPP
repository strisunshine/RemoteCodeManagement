///////////////////////////////////////////////////////////////
//                                                           //
//  HLIST.CPP -- heterogenious lists using handles           //
//                 list elements are not all derived         //
//                 from same inheritance hierarchy           //
//                                                           //
//  Modeled after "A Chameleon Class in C++", J.Crawford,    //
//  Computer Language, May '93 - fixed errors and simplified //
//                                                           //
///////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>
#include <iostream.h>

enum classType { APPLE, ORANGE };

//----< class Apples : ignorant of lists >---------------------

class Apples {

  public:
    Apples(char* descr=0, int age=0);
    Apples(const Apples&);
    ~Apples() { delete ApplesDescr; }
    Apples& operator=(const Apples&);
    void AppleSpeak() { cout << "I'm an apple\n"; }

  protected:
    char* ApplesDescr;
    int   ApplesAge;
};

Apples::Apples(char* descr, int age) : ApplesAge(age) {

  int len = strlen(descr);
  ApplesDescr = new char[len+1];
  assert(ApplesDescr != 0);
  int i=0;
  while ((ApplesDescr[i] = descr[i]) != 0) i++;
}

Apples::Apples(const Apples& apple) {

  int len = strlen(apple.ApplesDescr);
  ApplesDescr = new char[len+1];
  assert(ApplesDescr != 0);
  int i=0;
  while ((ApplesDescr[i] == apple.ApplesDescr[i]) != 0) i++;
}

Apples& Apples::operator=(const Apples& apple) {

  int len = strlen(apple.ApplesDescr);
  char* TempDescr = ApplesDescr;
  ApplesDescr = new char[len+1];
  assert(ApplesDescr != 0);
  int i=0;
  while ((ApplesDescr[i] = apple.ApplesDescr[i]) != 0) i++;
  delete TempDescr;
  return *this;
}
/**/
//----< class Oranges : Ignorant of lists >--------------------

class Oranges {

  public:
    Oranges(char* descr=0, int age=0);
    Oranges(const Oranges&);
    ~Oranges() { delete OrangesDescr; }
    Oranges& operator=(const Oranges&);
    void OrangeSpeak() { cout << "I'm an orange\n"; }

  protected:
    char* OrangesDescr;
    int   OrangesAge;
};

Oranges::Oranges(char* descr, int age) : OrangesAge(age) {

  int len = strlen(descr);
  OrangesDescr = new char[len+1];
  assert(OrangesDescr != 0);
  int i=0;
  while ((OrangesDescr[i] = descr[i]) != 0) i++;
}

Oranges::Oranges(const Oranges& orange) {

  int len = strlen(orange.OrangesDescr);
  OrangesDescr = new char[len+1];
  assert(OrangesDescr != 0);
  int i=0;
  while ((OrangesDescr[i] = orange.OrangesDescr[i]) != 0) i++;
}

Oranges& Oranges::operator=(const Oranges& orange) {

  int len = strlen(orange.OrangesDescr);
  char* TempDescr = OrangesDescr;
  OrangesDescr = new char[len+1];
  assert(OrangesDescr != 0);
  int i=0;
  while ((OrangesDescr[i] = orange.OrangesDescr[i]) != 0) i++;
  delete TempDescr;
  return *this;
}
/**/
//----< class HBase : communication link >---------------------
//
//  HBase is a private handle used to link the heterogeneous
//  objects into a new inheritance hierary, built to get the
//  benefits of dynamic dispatching for the heterogeneous case

class HBase {

  friend class HItem;

  protected:
    virtual    HBase* clone()     = 0;   // helps builds HItem elem
    virtual classType giveClass() = 0;   // identify class type
    virtual      void speak()     = 0;   // common message
    virtual ~HBase() {}
};

//----< class Apples2 : envelope for Apples and HBases >-------

class Apples2:private Apples, private HBase {

  friend class HItem;

  private:
    Apples2(const Apples& apple):Apples(apple) {}
    ~Apples2() {}

  // helps avoid mixing apples and oranges
    classType giveClass() { return type; }

  // return pointer to copy of an Apple in Apple2 guise
    HBase* clone()     { return new Apples2(*this); }

  // translation of HItem message to Apple message
    void speak() { Apples::AppleSpeak(); }

    static classType type;
};

classType Apples2::type = APPLE;

//

//----< class Oranges2 : envelope for Oranges and HBases >-----

class Oranges2:private Oranges, private HBase {

  friend class HItem;

  private:
    Oranges2(const Oranges& orange):Oranges(orange) {}
    ~Oranges2() {}

  // helps avoid mixing oranges and apples
    classType giveClass() { return type; }

  // return link to a copy of an Orange in Orange2 guise
    HBase* clone()     { return new Oranges2(*this); }

  // translation of HItem message to Orange message
    void speak() { Oranges::OrangeSpeak(); }

    static classType type;
};

classType Oranges2::type = ORANGE;
/**/
//----< class HItem : element of heterogeneous list >----------
//                      HItem needs to know a lot
//                      about objects in its list.

class HItem : private HBase {

  public:
    HItem()                  :ptrHBase(0) {}

  // copy construct list element using polymorphic cloning
    HItem(const HItem& Item) :ptrHBase(Item.ptrHBase->clone()) {}

  // construct Item element from an apple
    HItem(const Apples& apple)
                             :ptrHBase(new Apples2(apple)) {}

  // construct Item element from an orange
    HItem(const Oranges& orange)
                             :ptrHBase(new Oranges2(orange)) {}

    ~HItem() { delete ptrHBase; }

    HItem& operator=(const HItem&);

    operator Apples();

    operator Oranges();

  // polymorphic messaging
    void speak() { ptrHBase->speak(); }

  private:
    HBase* ptrHBase;
};

// assign one element to another
HItem& HItem::operator=(const HItem& Item) {
  if(this == &Item) return *this;
  delete ptrHBase;
  ptrHBase = Item.ptrHBase->clone();
  return *this;
}

// cast Item element to an Apple
HItem::operator Apples() {
  if(ptrHBase->giveClass()!=APPLE) {
    cout << "  attempt to make an apple into a non-apple\n";
    return "non-apple";        // can't mix apples and oranges
  }
  return *(Apples2*)ptrHBase;
}

// cast Item element to an Orange
HItem::operator Oranges() {
  if(ptrHBase->giveClass()!=ORANGE) {
    cout << "  attempt to make an orange into a non-orange\n";
    return "non-orange";       // can't mix oranges and apples
  }
  return *(Oranges2*)ptrHBase;
}
/**/
//----< main : Test Stub >-------------------------------------

void main() {

  cout << "\ntesting heterogeneous list\n";

  cout << "\nitems speaking:\n";

  Apples a("an apple");     a.AppleSpeak();
  Oranges o("an orange");   o.OrangeSpeak();

  cout <<
    "\nheterogeneous list elements speaking polymorphically:\n";

  const MAX = 4;
  HItem hl[MAX];
  hl[0] = a;  // note that an apple is being assigned to hl
  hl[1] = o;  // Now an orange is being assigned to the list
  hl[2] = a;
  hl[3] = o;

  for (int i=0; i<MAX; i++)
    hl[i].speak();

  o = hl[0];  // attempting to assign an apple to an orange

  o.OrangeSpeak();  // fortunately it didn't work

  a = hl[0];  // now we're assigning an apple to an apple

  a.AppleSpeak();   // fortunately it worked

}