#ifndef CLIENT_H
#define CLIENT_H
/////////////////////////////////////////////////////////////////////////
// Client.h - Used for testing concurrent communication, representing //
//             one of the clients that send messages and get replies   // 
//             from the servers. It can also disconnect with one       //
//             server and connect with another server.                 //            
//                                                                     //
// ver 1.0                                                             //                                                    
// Application: CSE687 2015 Project 4 - Remote Code Management         //
// Platform:    Win7, Visual Studio 2013                               //
// Author:      Wenting Wang, wwang34@syr.edu                          //                                            
/////////////////////////////////////////////////////////////////////////
/*
* Public Interface :
* == == == == == == == == =
* Message ms; size_t port = 7060;
* Client c(port)
* c.makeRequest(ms);
*
* Required Files :
*== == == == == == == =
* IClient.h, Sockets.h, Sockets.cpp, Message.h, Message.cpp, ISender.h, IRecvr.h,
*
* Build Process :
* == == == == == == ==
* devenv MessagePassing.sln / rebuild Debug
*
* Maintenance History :
* == == == == == == == == == ==
* Version 1.0 : 10 Apr 2015
* -first release
*/

#include <string>

#include "IClient.h"
#include "../Message/Message.h"
#include "../Sockets/Sockets/Sockets.h"
#include "../Receiver/IRecvr.h"
#include "../Sender/ISender.h"

class Client : public IClient{
public:
	//-----------< constructor >--------------------------------------------------------------------------
	Client(size_t port = 9080, std::string loc = "Client1Folder/", Socket::IpVer ipv = Socket::IP6, int qNum = 4, bool isRecvThrdEnded = true);

	// for original test
	/*bool connectTo(std::string ip, size_t port);*/

	//----------< configure its own listener port and ip protocol >--------------------------------
	bool configureListener(size_t port, Socket::IpVer ipv);

	//----------< users call this method to make a request, such as uploading a file, quitting >---
	void makeRequest(Message& ms){
		s->enQRequest(ms);
	};

	//----------< close its connection with a server that it's currently connected to >------------
	bool closeConnectionToServer(){
	/*	s.getSocketConnecter().shutDown();
		s.getSocketConnecter().close();*/
	};

	//----------< get a reference to its sender member object >------------------------------------
	ISender* getSender(){
		return s;
	}

	//----------< get a reference to its receiver member object >------------------------------------
	IReceiver* getReceiver(){
		return r;
	};

	~Client();
private:
	SocketSystem ss;
	ISender* s;
	IReceiver* r;
};

#endif