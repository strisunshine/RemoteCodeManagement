#ifndef ICLIENT_H
#define ICLIENT_H
/////////////////////////////////////////////////////////////////////////////
// IClient.h - CSE 687 Project #4, 2015                                    //
// - build as DLL so that C++\CLI client can use native code to reads      //
// - reads from sendQ and writes to recvQ                                  //
//                                                                         //
// Language:    Visual C++, Visual Studio 2013                             //
// platform:    Windows 7                                                  //
// Application: CSE687 2015 - Project 4 - Remote Code Management           //
// Author:      Wenting Wang, wwang34@syr.edu                              //
/////////////////////////////////////////////////////////////////////////////

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include "../Message/Message.h"
#include "../Receiver/IRecvr.h"
#include "../Sender/ISender.h"

struct IClient
{
	virtual void makeRequest(Message& ms) = 0;
	virtual ISender* getSender() = 0;
	virtual IReceiver* getReceiver() = 0;
};

extern "C" {
	struct ObjectFactory
	{
		DLL_DECL IClient* createClient(size_t port, std::string loc);
	};
}

#endif