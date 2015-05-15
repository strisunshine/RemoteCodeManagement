#ifndef SENDER_H
#define SENDER_H

/////////////////////////////////////////////////////////////////////
// Sender.h - Support sending messages and replies to remote end   //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 3 - Message Passing            //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains one class, Sender.
*
* The Sender class support sending messages or replies to a remote 
* end. It has a blocking queue, either sender or other classes can
* enque messages into its queue. Its sender thread is constantly 
* pulling messages from the queue. If no message exist, the queue 
* blocks untill a new message arrives.
* It also contains methods to query the state of the sender thread.
*
* Public Interface:
* =================
* Sender s; Message ms;
* ms.configureFileUploadMsg("test.txt", "::1@9080", "::1@8090");
* s.sendFile(ms);
* ms.configureQuitMsg("::1@9080", "::1@6070");
* s.sendQuitMsg(ms);
* s.enQRequest(ms)
* s.deQRequest();
* s.sendThreadStateValid();
*
* Required Files:
* ===============
* ISender.h, Sockets.h, Sockets.cpp, Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp, Message.h, Message.cpp,
* AppHelpers.h, WindowsHelpers.h
*
* Build Process:
* ==============
* devenv MessagePassing.sln /rebuild Debug
*
* Maintenance History:
* ====================
* Version 1.0 : 10 Apr 2015
* - first release
*/

#include "ISender.h"
#include "../Sockets/Sockets/Sockets.h"
#include "../ApplicationHelpers/AppHelpers.h"
#include "../WindowsHelpers/WindowsHelpers.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Message/Message.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <iostream>
#include <sstream>

class Sender : public ISender{
public:
	Sender();;
	Sender(const Sender& s) = delete;
	Sender& operator=(const Sender& s) = delete;
	Sender(Sender&& s);;
	Sender& operator=(Sender&& s);

	void enQRequest(Message& ms);
	Message peekRequest();
	void deQRequest();;

	// used to check to make new connection
	bool checkOrMakeConnection(Message& ms);
	// used by the client
	bool sendFile(Message ms);
	bool sendFileSearchMsg(Message& ms);
	bool sendTextSearchMsg(Message& ms);
	bool sendQuitMsg(Message& ms);

	// used by the server
	bool sendFileUploadReplyorDownLoadMsg(Message& ms);
	bool sendFileSearchReply(Message& ms);
	bool Sender::sendTextSearchReply(Message& ms);
	bool sendQuitReply(Message& ms);

	// query the sender thread state
	bool sendThreadStateValid();

	SocketConnecter& getSocketConnecter();
	~Sender();
private:
	bool sendFileblockHeader(Message& ms);
	SocketConnecter socketconn;
	BlockingQueue<Message> senderQ;
	//represent the address the send is currently connected to
	std::string connectedAddress;
	std::mutex mtx_;
	//flag for if the sender thread is ended or not
	bool isSenderThreadEnded = false;
	
};


#endif