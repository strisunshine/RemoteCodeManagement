#ifndef RECEIVER_H
#define RECEIVER_H

/////////////////////////////////////////////////////////////////////
// Receiver.h - Support receiving messages and dispatching them to //
//              different processing mechanism                     //
// ver 1.0                                                         //                                                    
// Application: CSE687 2015 Project 3 - Message Passing            //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains two classes, Receiver and ClientHandler.
*
* The Receiver class support receiving messages enqueued by client 
* handlers. It will then dispatch the messages to different 
* processing mechanisms for further processing. If any reply or 
* result is needed by the client, the receiver or different process
* mechanisms will enqueue those replies or results to the sender
* queue of the sender that belongs to the same server.
* The Receiver's queue is a blocking queue, client handers enqueue
* messages in and its receiver thread that runs since the class's
* construction is constantly pulling messages from the queue. 
* It also contains methods to query the state of the receiver thread.
*
* The ClientHandler class support imediate receiving of messages 
* from the sockets. It then dispatches the messages using a class
* functor depending on the command type it receives to its member
* methods. Inside these member methods, the received messages are 
* parsed into Message class objects and enqueued into the 
* corresponding receiver queues. For file uploads, the files are
* stored to the server space before a message is enqueued to the
* receiver.
* 
* Public Interface:
* =================
* Socket sc; Message ms; Receiver r; ClientHandler ch;
* r.enQRequest(ms)
* r.deQRequest();
* r.receiveThreadStateValid();
* ch(sc, r);
* ch.fileUploadHandling(sc, r);
* ch.quitReplyHandling(sc, r);
* 
* Required Files:
* ===============
* Sockets.h, Sockets.cpp, Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp, Message.h, Message.cpp, Sender.h, Sender.cpp,
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

#include "IRecvr.h"
#include "../Sockets/Sockets/Sockets.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Message/Message.h"
#include "../Sender/Sender.h"
#include "../FileCatalogue/FileCatalogue.h"
#include "../ApplicationHelpers/AppHelpers.h"
#include "../WindowsHelpers/WindowsHelpers.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <set>
#include <map>

class ClientHandler
{
public:
	// used by the server side to handle client's requests
	bool fileUploadorFileDownloadReplyHandling(Socket& socket_, Receiver& rcv); //used by the client for file download reply as well
	bool fileSearchHandling(Socket& socket_, Receiver& rcv);
	bool textSearchHandling(Socket& socket_, Receiver& rcv);
	bool fileDownloadHandling(Socket& socket_, Receiver& rcv);
	bool quitMsgHandling(Socket& socket_, Receiver& rcv);
	
	// used by the client side to handle server's replies
	bool fileUploadReplyHandling(Socket& socket_, Receiver& rcv);
	bool fileSearchReplyHandling(Socket& socket_, Receiver& rcv);
	bool textSearchReplyHandling(Socket& socket_, Receiver& rcv);
	bool quitReplyHandling(Socket& socket_, Receiver& rcv);

	// operator() for dispatching handling to different class methods 
	// based on command type
	void operator()(Socket& socket_, Receiver& rcv);
private:
	// private helper function for check whether the file is in processing 
	bool openFileforProcessing(std::string& fullfilename, std::string& fileName, Socket& sc, std::ofstream& ofs);
	bool writeToFileBufferData(bool& ok, char * buffer, int BufLen, Socket& sc, std::ofstream& ofs);
	// get the Upload/ FileDownload Reply Message Attributes
	void findUploadorFileDownloadReplyAttributes(Socket& socket_, std::string& srcAddress, std::string& dstAddress, std::string&  mode, std::string& servernumber, std::string& serverrelativepath);
	void findFileSearchReplyAttributes(Socket& socket_, std::string& servernumber, std::string& numberoffiles, std::string& numberofdirectories, std::string& srcAddress, std::string& dstAddress, std::string& asxml, std::string& patterns, std::string& serverrelativepath, std::string& mode);
	std::mutex mtx_;
	std::condition_variable cv_;
	// for original test
	/*std::set<std::string> FilesInProcessing;*/
	// contains mapping from file names to the sockets that handling their uploading
	// used to check if a file with the same name and path is already being written to by another thread
	std::map<std::string, Socket*> FileSocketPairsInProcessing;
};

class Receiver : public IReceiver{
public:
	Receiver(size_t port, std::string loc, Socket::IpVer ipv, int qNum, Sender* s = new Sender(), bool isRecvThrdEnded = false);
	Receiver(const Receiver& r) = delete;
	Receiver& operator=(const Receiver& r) = delete;
	
	void enQRequest(Message& ms);
	Message deQSingleRequest();
	void deQRequest();
	
	// dispatches for different messages
	void recvFileUploadDispath(Message& ms);
	void recvFileSearchDispath(Message& ms);
	void recvTextSearchDispath(Message& ms);
	void constructFileListXmlBody(Message& ms, std::vector<std::string>& resfiles);
	void recvFileDownloadDispath(Message& ms);
	void recvFileUploadReplyDispath(Message& ms);
	void recvFileDownloadReplyDispath(Message& ms);

	// query the send thread state
	bool receiveThreadStateValid();

	// get the registration number of specific instances that are using the receiver
	int getNum();

	// get the folder where the server/client resides
	std::string getLoc();

	// get the Socket Listener
	SocketListener& getSocketListener();

	// destructor
	~Receiver();
private:
	// represent the registration number of the client/server that 
	// is using the receiver 
	int qNum_;
	SocketListener socketlstn;
	// a functor dispatcher of message requests
	ClientHandler ch;
	Sender* s_;
	// used for orinal test while using Qserver, register different client
	// or server with different number
	/*QServer<Message, 1> receiver1Q;
	QServer<Message, 2> receiver2Q;
	QServer<Message, 3> receiver3Q;
	QServer<Message, 4> receiver4Q;*/
	BlockingQueue<Message> receiverQ;
	// flag for if the receiver thread is ended or not
	bool isReceiveThreadEnded = false;
	std::string loc_;
};

#endif