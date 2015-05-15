///////////////////////////////////////////////////////////////
// Structure.cpp - Demonstrate how ATL processing works      //
//                                                           //
// Jim Fawcett, CSE775 - Distributed Objects, Spring 2008    //
///////////////////////////////////////////////////////////////

#include "Structure.h"

void main()
{
  std::cout << "\n  Demo ATL Style Processing ";
  std::cout << "\n ===========================\n";

  // use object factory to create instance of component

  IComponent* pComp = ICreatable<IComponent>::Create();
  
  // get a standard interface and do some standard processing

  IStdProcessingImpl* pStdProc;
  pComp->QueryInterface(&pStdProc);
  pStdProc->doStdProcessing();
  
  // do this component's processing

  pComp->compProcessing();
  
  // ok, done with component

  pComp->Release();
}


