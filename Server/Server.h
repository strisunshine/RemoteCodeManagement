#ifndef SERVER_H
#define SERVER_H
/////////////////////////////////////////////////////////////////////////
// Server.h - Used for testing concurrent communication, representing //
//             one of the servers that can receive and process messages// 
//             sent by different clients concurrently and send back    //
//             replies to the clients                                  //          
//                                                                     //
// ver 1.0                                                             //                                                    
// Application: CSE687 2015 Project 3 - Message Passing                //
// Platform:    Win7, Visual Studio 2013                               //
// Author:      Wenting Wang, wwang34@syr.edu                          //                                            
/////////////////////////////////////////////////////////////////////////
/*
* Public Interface :
* == == == == == == == == =
* Message ms; size_t port = 7060;
* Server s(port);
*
* Required Files :
*== == == == == == == =
* Sockets.h, Sockets.cpp, Message.h, Message.cpp, Sender.h, Sender.cpp, Receiver.h, Receiver.cpp, AppHelpers.h
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

#include "../Sockets/Sockets/Sockets.h"
#include "../Message/Message.h"
#include "../Receiver/Receiver.h"
#include "../Sender/Sender.h"
#include "../ApplicationHelpers/AppHelpers.h"

#include <string>

class Server{
public:
	Server(size_t port = 8090, std::string loc = "Server1Folder/", Socket::IpVer ipv = Socket::IP6, int qNum = 1) : r(new Receiver(port, loc, ipv, qNum, s)){}

	//----------< configure its own listener port and ip protocol >--------------------------------
	bool configureListener(size_t port, Socket::IpVer ipv){
		r->getSocketListener() = std::move(SocketListener(port, ipv));
	};

	//----------< users call this method to make a request, such as uploading a file, quitting >---
	void makeRequest(Message& ms){
		s->enQRequest(ms);
	};

	//----------< close its connection with a server that it's currently connected to >-----------
	bool closeConnectionToServer(){ return false; };

	// for original test
	/*bool closeConnection(std::string ip);*/
	//-----------< destructor >----------------------------------------------------------
	~Server(){
		delete s;
		delete r;
	}
private:
	Sender* s = new Sender();
	Receiver* r;
};

#endif