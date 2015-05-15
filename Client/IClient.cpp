/////////////////////////////////////////////////////////////////////////////
// IClient.cpp - CSE 687 Project #4, 2015                                  //
// - build as DLL so that C++\CLI client can use native code to reads      //
// - reads from sendQ and writes to recvQ                                  //
//                                                                         //
// Language:    Visual C++, Visual Studio 2013                             //
// platform:    Windows 7                                                  //
// Application: CSE687 2015 - Project 4 - Remote Code Management           //
// Author:      Wenting Wang, wwang34@syr.edu                              //
/////////////////////////////////////////////////////////////////////////////

#include "IClient.h"
#include "Client.h"

//----< factory functions >--------------------------------------------------
IClient* ObjectFactory::createClient(size_t port, std::string loc)
{
	return new Client(port, loc);
}
