/////////////////////////////////////////////////////////////////////
// Sender.cpp - Support sending messages and replies to remote end //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 3 - Message Passing            //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////

#include "Sender.h"
#include "..\FileCatalogue\FileSystem.h"

using namespace ApplicationHelpers;
using namespace WindowsHelpers;

//----------< default constructor >--------------------------------------------------
//------< a thread is created that pulls requests from the sender blocking queue >---
Sender::Sender(){
	std::thread t([this] { this->deQRequest(); });
	t.detach();
	Verbose::show("Sender is being created \n");
};

//----------< move constructor >-----------------------------------------------------
Sender::Sender(Sender&& s){
	socketconn = SocketConnecter(std::move(s.socketconn));
	Verbose::show("Sender is being moved \n", always);
};

//----------< assignment operator >--------------------------------------------------
Sender& Sender::operator=(Sender&& s){
	if (this == &s) return *this;
	s.socketconn = SocketConnecter(std::move(s.socketconn));
	Verbose::show("Sender is being move assigned \n", always);
	return *this;
};

//----------< enque a message to the sender queue >--------------------------------
void Sender::enQRequest(Message& ms){
	senderQ.enQ(ms);
};

//----------< peek a message from the sender queue >--------------------------------
Message Sender::peekRequest(){
	return senderQ.peek();
}

//--------< deque a message from the sender queue and send it >---------------------
//--------< it loops in a thread created in the sender constructor >----------------
//--------< the loop breaks only when the it receives a quit message >--------------
void Sender::deQRequest(){
	while (!isSenderThreadEnded){
		Message ms = senderQ.deQ();
		if(!checkOrMakeConnection(ms)) continue; //if connection fails, skips this message and goes to fetch next one	
		// switch condition used to dispatch different commands to different sending mechanism
		switch (ms.getCommandNumberMap()[ms.getCommand()]){
			case 1:
			case 6:
				sendFile(ms);
				break;
			case 3:
				sendFileSearchMsg(ms);
				break;
			case 4:
				sendFileSearchReply(ms);
				break;
			case 2:
			case 5:
				sendFileUploadReplyorDownLoadMsg(ms);
				break;
			case 7:
				sendTextSearchMsg(ms);
				break;
			case 8:
				sendTextSearchReply(ms);
				break;
		}
	};
};

bool Sender::checkOrMakeConnection(Message& ms){
	std::string dstAddr = ms.getAttributes()["dstAddress"];
	//if the sender is already connected to an address same with the destination address of this request, then no need to reconnect
	//if (connectedAddress != dstAddr){
		std::string ip = dstAddr.substr(0, dstAddr.find('@'));
		size_t port = toOrginType<size_t>(dstAddr.substr(dstAddr.find('@') + 1));
		bool connRes = false;
		int count = 0;
		while (!connRes && count <10){ //try 10 times to connect
			connRes = socketconn.connect(ip, port);
			count++;
			Sleep(100);
		}
		if (connRes){
			connectedAddress = dstAddr;
			Verbose::show("-- The Client/Server itself is currently connected to " + dstAddr + "\n", always);
		}
		else{
			connectedAddress = ""; 
			Verbose::show("-- Attempt to connect to " + dstAddr + " failed, message skipped\n", always);
			return false;
		}
	//}
	//else Verbose::show("Connection to " + dstAddr + " is the same with last time's: " + connectedAddress, always);
	return true;
}
//----------< used by client or server to send a whole file >-----------------------------------
bool Sender::sendFile(Message ms){
	bool ok = true;
	//Verbose::show("Plan to open the file " + ms.getFiletoUpload(), always);
	std::string fullpath = FileSystem::Path::fileSpec(ms.getAttributes()["path"], ms.getFiletoUpload());
	std::ifstream ifs(fullpath, std::ifstream::binary);
	//Verbose::show("The ifstream is opened for: " + ms.getFiletoUpload(), always);
	size_t len = ms.getFileUploadBlockLength();
	//Verbose::show("Will go into the while loop inside sendFile for " + ms.getFiletoUpload(), always);
	while (true){
		//std::lock_guard<std::mutex> l(mtx_);
		char * buffer = new char[len];
		ifs.read(buffer, len);

		//if the content is less then the block length, which means it's the last block
		size_t realLen = (size_t)ifs.gcount(); 
		if (realLen < len){
			ms.setFileUploadBlockLength(realLen);
			//Verbose::show("Is sending the last header string for " + ms.getCommand() + " of " + ms.getFiletoUpload(), always);
			ok = ok && sendFileblockHeader(ms);
			ok = ok && socketconn.send(realLen, buffer);
			delete[] buffer;
			break;
		}
		else{
			ok = ok && sendFileblockHeader(ms);
			ok = ok && socketconn.send(len, buffer);
			delete[] buffer;
		}

		//if the file length happens to be a multiple of the block length
		if (ifs.eof()){
			/*buffer = new char[len];*/
			//Verbose::show("Is sending the last header string for " + ms.getCommand() + " of " + ms.getFiletoUpload(), always);
			ms.setFileUploadBlockLength(0);
			ok = ok && sendFileblockHeader(ms);
			/*socketconn.send(len, buffer);
			delete[] buffer;*/
			break;
		}
	}
	ifs.close();
	if (ok) return true;
	else{
		Verbose::show("There is some error in sending the file: " + ms.getFiletoUpload() + " \n", always);
		return false;
	}
};

//----------< used by client to send a file search request >-----------------------
bool Sender::sendFileSearchMsg(Message& ms){
	bool ok = true;
	socketconn.sendString(ms.getCommand());
	for (auto attrib : ms.getAttributes()){
		socketconn.sendString(attrib.first + ':' + attrib.second);
	}
	socketconn.sendString("\n");
	if (ok) return true;
	else{
		Verbose::show("There is error in sending file search message \n", always);
		return false;
	}
}

//----------< used by client to send a text search request >-----------------------
bool Sender::sendTextSearchMsg(Message& ms){
	bool ok = true;
	socketconn.sendString(ms.getCommand());
	for (auto attrib : ms.getAttributes()){
		socketconn.sendString(attrib.first + ':' + attrib.second);
	}
	socketconn.sendString("\n");
	if (ok) return true;
	else{
		Verbose::show("There is error in sending text search message \n", always);
		return false;
	}
}

//----------< used by client to send a quit request >------------------------------
bool Sender::sendQuitMsg(Message& ms){
	bool ok = true;
	socketconn.sendString(ms.getCommand());
	for (auto attrib : ms.getAttributes()){
		socketconn.sendString(attrib.first + ':' + attrib.second);
	}
	socketconn.sendString("\n");
	if (ok) return true;
	else{
		Verbose::show("There is error in sending quit message \n", always);
		return false;
	}
};

//----------< used by server to send a reply to file upload request  >--------------
//----------< or used by client to send a file download request >-------------------
bool Sender::sendFileUploadReplyorDownLoadMsg(Message& ms){
	if (sendFileblockHeader(ms)) return true;
	else{
		Verbose::show("There is error in sending " + ms.getCommand() + " \n", always);
		return false;
	}
};

//----------< used by server to send a reply to client's file search request >--------
bool Sender::sendFileSearchReply(Message& ms){
	bool ok = true;
	socketconn.sendString(ms.getCommand());
	for (auto attrib : ms.getAttributes()){
		socketconn.sendString(attrib.first + ':' + attrib.second);
	}
	socketconn.sendString("\n");	
	if (ms.getAttributes()["numberoffiles"] == "0" && ms.getAttributes()["asxml"] == "False") socketconn.sendString("\n");
	else socketconn.sendString(ms.getBody());
	if (ok) return true;
	else{
		Verbose::show("There is error in sending file search reply \n", always);
		return false;
	}
};

//----------< used by server to send a reply to client's text search request >--------
bool Sender::sendTextSearchReply(Message& ms){
	bool ok = true;
	socketconn.sendString(ms.getCommand());
	for (auto attrib : ms.getAttributes()){
		socketconn.sendString(attrib.first + ':' + attrib.second);
	}
	socketconn.sendString("\n");

	if (ms.getAttributes()["numberoffiles"] == "0" && ms.getAttributes()["asxml"] == "False") socketconn.sendString("\n");
	else socketconn.sendString(ms.getBody());
	if (ok) return true;
	else{
		Verbose::show("There is error in sending text search reply \n", always);
		return false;
	}
};

//----------< used by server to send a reply to client's quit request >------------
bool Sender::sendQuitReply(Message& ms){
	if (sendQuitMsg(ms)) return true;
	else{
		Verbose::show("There is error in sending quit message reply \n", always);
		return false;
	}
}

//----------< check if the sender thread state is valid >----------------------------
bool Sender::sendThreadStateValid(){
	return isSenderThreadEnded == false;
}

//----------< get the reference to the socket connector >-----------------------------
SocketConnecter& Sender::getSocketConnecter(){ return socketconn; };

//----------< destructor >------------------------------------------------------------
Sender::~Sender(){
	socketconn.shutDown();
	socketconn.close();
}

//----------< a private function to send the header for each file block >-------------
bool Sender::sendFileblockHeader(Message& ms){
	bool ok = true;
	//std::string dstAddress = ms.getAttributes()["dstAddress"];
	socketconn.sendString(ms.getCommand());
	socketconn.sendString(ms.getFiletoUpload());
	socketconn.sendString(toString(ms.getFileUploadBlockLength()));

	socketconn.sendString(toString(ms.getClientSenttime().time_since_epoch().count()));
	
	if (ms.getServerRecvtime().time_since_epoch().count() != 0){
		socketconn.sendString(toString(ms.getServerRecvtime().time_since_epoch().count()));
		socketconn.sendString(toString(ms.getServerSenttime().time_since_epoch().count()));
	}
	for (auto attrib : ms.getAttributes()){
		socketconn.sendString(attrib.first + ':' + attrib.second);
	}
	socketconn.sendString("\n");
	if (ok) return true;
	else{
		Verbose::show("There is error in sending the file block header for " + ms.getFiletoUpload() + " \n");
		return false;
	}
};

#ifdef TEST_SENDER
int main()
{
	try
	{
		Verbose v(true);
		SocketSystem ss;
		Sender s;
		Message ms;
		ms.configureFileUploadMsg("client1toServer1First.txt", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		ms.configureFileUploadMsg("pr1s15.pdf", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		/*ms.configureFileUploadMsg("C+++How+to+Program+Fifth.chm", "::1@9080", "::1@8090");
		s.enQRequest(ms);*/
		ms.configureFileUploadMsg("kola.jpg", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		ms.configureFileUploadMsg("science.txt", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		ms.configureFileUploadMsg("TestFileCatalogue.exe", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		ms.configureFileUploadMsg("MT4S03sol.doc", "::1@9080", "::1@8090");
		s.enQRequest(ms);	
		ms.configureQuitMsg("::1@9080", "::1@8090");
		s.enQRequest(ms);
		while (s.sendThreadStateValid());
	}
	catch (std::exception& ex)
	{
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
	}
	std::cout << "\n  press key to exit: ";
}
#endif