///////////////////////////////////////////////////////////////
// Structure.h - Demonstrate how ATL processing works        //
//                                                           //
// Jim Fawcett, CSE775 - Distributed Objects, Spring 2008    //
///////////////////////////////////////////////////////////////

#include <iostream>

/////////////////////////////////////////////////////
// object factory

template <typename T>
struct ICreatable
{
  static T* Create();
  virtual void AddRef()=0;
  virtual void Release()=0;
  virtual ~ICreatable() {}
};

/////////////////////////////////////////////////////
// one of many standard processing facilities

struct IStdProcessingImpl
{
  virtual void doStdProcessing()=0;
  virtual ~IStdProcessingImpl() {}
};

class StdProcessing : public IStdProcessingImpl
{
public:
  void doStdProcessing()
  {
    std::cout << "\n  doing standard processing";
  }
};

/////////////////////////////////////////////////////
// component's interface

struct IUnknown
{
  virtual void AddRef()=0;
  virtual void Release()=0;
  virtual void QueryInterface(IStdProcessingImpl** ppStdProc)=0;
};

struct IComponent : public IUnknown
{
  virtual void compProcessing()=0;
};

/////////////////////////////////////////////////////
// the component you think you are designing

class aComponent : public IComponent, public ICreatable<aComponent>, public IStdProcessingImpl
{
public:
  void compProcessing()
  {
    std::cout << "\n  doing component processing";

    // use std processing as part of my implementation

    doStdProcessing();
  }
};

/////////////////////////////////////////////////////
// theRealComponent adds IUnknown methods, 
// defines creation, adds standard processing

class theRealComponent : public aComponent
{
public:
  theRealComponent() : refCount(0) {}

  //----< implements IUnknown methods >------------------------

  void AddRef() 
  {
    ++refCount; 
  };
  void Release() 
  {
    if(--refCount == 0)
    {
      std::cout << "\n  component destroyed\n\n";
      delete this;
    }
  }
  void QueryInterface(IStdProcessingImpl** ppStdProc)
  {
    // note cross-cast

    IStdProcessingImpl* pStdProc = dynamic_cast<IStdProcessingImpl*>(this);
    if(!pStdProc)
      throw "no such interface";
    *ppStdProc = pStdProc; 
  }
  //----< implements IStdProcessingImpl::doStdProcessing >-----

  void doStdProcessing()
  {
    StdProcessing().doStdProcessing();
  }
private:
  int refCount;
};

//----< Create creates theRealComponent, not aComponent >----//

IComponent* ICreatable<IComponent>::Create()
{
  IComponent* pComp = new theRealComponent();
  pComp->AddRef();
  std::cout << "\n  component created";
  return pComp;
}

