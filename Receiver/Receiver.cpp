///////////////////////////////////////////////////////////////////////
// Receiver.cpp - Support receiving messages and dispatching them to //
//              different processing mechanism                       //
// ver 1.0                                                           //                                                    
// Application: CSE687 2015 Project 3 - Message Passing              //
// Platform:    Win7, Visual Studio 2013                             //
// Author:      Wenting Wang, wwang34@syr.edu                        //                                            
///////////////////////////////////////////////////////////////////////

#include <sstream>

#include "Receiver.h"
#include "../XmlDocument/XmlDocument.h"

using namespace ApplicationHelpers;
using namespace WindowsHelpers;
using namespace FileCatalogueNS;
using namespace XmlProcessing;

//----< Client Handler thread starts running this function >-------------------
//----< the socket_ is created for the communication with a specific sender >--
void ClientHandler::operator()(Socket& socket_, Receiver& rcv)
{
	Verbose::show("Begining of CH operator()");
	while (true)
	{
		// interpret received command
		std::string command = socket_.recvString();
		Verbose::show("command: " + command);
		Verbose::show("WSAgetlastmsg: " + GetLastMsg(false));
		if (command.size() == 0)
			break;

		Message ms;
		switch (ms.getCommandNumberMap()[command]){
		case 1:
		case 6:
			fileUploadorFileDownloadReplyHandling(socket_, rcv);
			break;
		case 2:
			fileUploadReplyHandling(socket_, rcv);
		case 3:
			fileSearchHandling(socket_, rcv);
			break;
		case 4:
			fileSearchReplyHandling(socket_, rcv);
			break;
		case 5:
			fileDownloadHandling(socket_, rcv);
			break;
		case 7:
			textSearchHandling(socket_, rcv);
			break;
		case 8:
			textSearchReplyHandling(socket_, rcv);
			break;
		}
	}
	// we get here if command isn't requesting a processing, e.g., client receive server's "QUIT_REPLY"
	Verbose::show("-- ClientHandler socket connection closing\n", always);
	socket_.shutDown();
	socket_.close();
	Verbose::show("-- ClientHandler thread terminating\n", always);
};

//---------< handling client's file upload request or server's file download reply msg>----------------
bool ClientHandler::fileUploadorFileDownloadReplyHandling(Socket& socket_, Receiver& rcv)
{
	bool ok;
	std::string filename = socket_.recvString();
	size_t contentLen = toOrginType<size_t>(socket_.recvString());
	std::chrono::time_point<std::chrono::high_resolution_clock> clientSenttime = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::microseconds(toOrginType<unsigned long long>(socket_.recvString()))/10);;
	std::string path;
	if (rcv.getNum() == 1 || rcv.getNum() == 2) path = "./" + rcv.getLoc() +"Upload/";
	else path = "./" + rcv.getLoc() + "Download/";
	std::string fullfilename = path + filename;
	std::ofstream ofs;
	char origindirectory[500];
	::GetCurrentDirectoryA(500, origindirectory);
	::SetCurrentDirectoryA(path.c_str());
	openFileforProcessing(fullfilename, filename, socket_, ofs);
	if (contentLen == 1024)
	{
		const int BufLen = 1024;
		char * buffer = new char[BufLen];
		while (socket_.recvString() != "\n");
		if (!writeToFileBufferData(ok, buffer, contentLen, socket_, ofs)) return false;
		delete[] buffer;
	}
	else{
		// this is the last block of the file
		std::string srcAddress, dstAddress, attribStr, mode, servernumber, serverrelativepath, localpath = path;
		findUploadorFileDownloadReplyAttributes(socket_, srcAddress, dstAddress, mode, servernumber, serverrelativepath);	
		// if the file block is not empty
		if (contentLen > 0){
			char * buffer = new char[contentLen];
			if (!writeToFileBufferData(ok, buffer, contentLen, socket_, ofs)) return false;
			delete[] buffer;
		}
		// remove the filename and socket pair from the FileSocketPairsInProcessing map
		FileSocketPairsInProcessing.erase(fullfilename);
		// if used by server to handle FILE_UPLOAD, construct a message and push onto the server's receiver's queue
		if (rcv.getNum() == 1 || rcv.getNum() == 2){
			Message file_upload_toprocess;
			file_upload_toprocess.configureFileUploadMsg(filename, srcAddress, dstAddress, path, mode);
			file_upload_toprocess.setClientSenttime(clientSenttime);
			file_upload_toprocess.setServerRecvtime(std::chrono::high_resolution_clock::now());
			rcv.enQRequest(file_upload_toprocess);
		}
		else{ // used by client to handle server's file download request reply, construct a message and push onto the client's receiver's queue
			Message file_download_reply;
			file_download_reply.configureFileDownloadReply(filename, srcAddress, dstAddress, servernumber, serverrelativepath, localpath, mode);
			rcv.enQRequest(file_download_reply);
		}
	}
	ofs.close();
	::SetCurrentDirectoryA(origindirectory);
	return true;
};

//---------< get the Upload/ FileDownload Reply Message Attributes >-------
void ClientHandler::findUploadorFileDownloadReplyAttributes(Socket& socket_, std::string& srcAddress, std::string& dstAddress, std::string&  mode, std::string& servernumber, std::string& serverrelativepath){
	std::string attribStr;
	// reverse the source address and destination address and find other attributes
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "mode"){
			mode = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "servernumber"){
			servernumber = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "path"){
			serverrelativepath = (attribStr.substr(attribStr.find(':') + 1));
		}
	}
}

//---------< get the File Search Reply Message Attributes >-------
void ClientHandler::findFileSearchReplyAttributes(Socket& socket_, std::string& servernumber, std::string& numberoffiles, std::string& numberofdirectories, std::string& srcAddress, std::string& dstAddress, std::string& asxml, std::string& patterns, std::string& serverrelativepath, std::string& mode){
	// reverse the source address and destination address and collect all the other attributes
	std::string attribStr;
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "asxml"){
			asxml = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "mode"){
			mode = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "numberoffiles"){
			numberoffiles = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "numberofdirectories"){
			numberofdirectories = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "servernumber"){
			servernumber = (attribStr.substr(attribStr.find(':') + 1));
		}
	}
}


//---------< handling client's file search request >---------------------
bool ClientHandler::fileSearchHandling(Socket& socket_, Receiver& rcv){
	Message file_search_msg;
	std::string srcAddress, dstAddress, attribStr, asxml, serverrelativepath, patterns;
	// reverse the source address and destination address
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "asxml"){
			asxml = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "patterns"){
			patterns = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "serverrelativepath"){
			serverrelativepath = (attribStr.substr(attribStr.find(':') + 1));
			serverrelativepath = rcv.getLoc() + serverrelativepath;
		}
	}

	// configure a "FILESEARCH" message and enque into the server's receiver
	file_search_msg.configureFileSearchMsg(srcAddress, dstAddress, asxml, patterns, serverrelativepath);
	rcv.enQRequest(file_search_msg);
	return true;
}

//---------< handling client's text search request >---------------------
bool ClientHandler::textSearchHandling(Socket& socket_, Receiver& rcv){
	Message text_search_msg;
	std::string srcAddress, dstAddress, attribStr, asxml, threads, patterns, searchtext;
	// reverse the source address and destination address
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "asxml"){
			asxml = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "patterns"){
			patterns = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "searchtext"){
			searchtext = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "threads"){
			threads = (attribStr.substr(attribStr.find(':') + 1));
		}
	}

	// configure a "FILESEARCH" message and enque into the server's receiver
	text_search_msg.configureTextSearchMsg(srcAddress, dstAddress, asxml, patterns, searchtext, threads);
	rcv.enQRequest(text_search_msg);
	return true;
}

//---------< handling client's file download request >-------------------
bool ClientHandler::fileDownloadHandling(Socket& socket_, Receiver& rcv){
	Message ms;
	size_t contentLen;
	std::string filename = socket_.recvString();
	contentLen = toOrginType<size_t>(socket_.recvString());
	std::string srcAddress, dstAddress, attribStr, serverrelativepath, mode;

	// reverse the source address and destination address and collect all the other attributes
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "path"){
			serverrelativepath = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "mode"){
			mode = (attribStr.substr(attribStr.find(':') + 1));
		}
	}
	// construct a message and push onto the server's receiver's queue
	Message file_download_toprocess;
	file_download_toprocess.configureFileDownloadMsg(filename, srcAddress, dstAddress, serverrelativepath, mode, contentLen);
	rcv.enQRequest(file_download_toprocess);
	return true;
}

//---------< handling client's quit request >-------------------------
bool ClientHandler::quitMsgHandling(Socket& socket_, Receiver& rcv){
	Message quit_msg;
	std::string srcAddress, dstAddress, attribStr;

	// reverse the source address and destination address
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
	}

	// configure a "QUIT" message and enque into the server's receiver
	quit_msg.configureQuitMsg(srcAddress, dstAddress);
	rcv.enQRequest(quit_msg);
	return true;
};

//---------< handling server's quit request reply>-------------------------
bool ClientHandler::quitReplyHandling(Socket& socket_, Receiver& rcv){
	Message ms;
	std::string srcAddress, dstAddress, attribStr;

	// reverse the source address and destination address
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
	}
	// configure a "QUIT_REPLY" message and enque into the client's receiver
	ms.configureQuitMsg(srcAddress, dstAddress);
	ms.setCommand("QUIT_REPLY");
	rcv.enQRequest(ms);
	return true;
};

//---------< handling server's file upload request reply>----------------------
bool ClientHandler::fileUploadReplyHandling(Socket& socket_, Receiver& rcv){
	Message ms;
	std::string result, servernumber, path, filename;
	size_t contentLen;
	filename = socket_.recvString();
	contentLen = toOrginType<size_t>(socket_.recvString());
	std::chrono::time_point<std::chrono::high_resolution_clock> clientSenttime, serverRecvtime, serverSenttime;

	clientSenttime = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::microseconds(toOrginType<unsigned long long>(socket_.recvString())) / 10);;
	serverRecvtime = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::microseconds(toOrginType<unsigned long long>(socket_.recvString())) / 10);;
	serverSenttime = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::microseconds(toOrginType<unsigned long long>(socket_.recvString())) / 10);;
	std::string srcAddress, dstAddress, attribStr, mode;

	// reverse the source address and destination address and collect all the other attributes
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "mode"){
			mode = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "result"){
			result = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "servernumber"){
			servernumber = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "path"){
			path = (attribStr.substr(attribStr.find(':') + 1));
		}
	}
	// configure a "FILE_UPLOAD_REPLY" message and enque into the client's receiver
	Message file_upload_reply;
	std::chrono::time_point<std::chrono::high_resolution_clock> clientRecvtime = std::chrono::high_resolution_clock::now();
	file_upload_reply.configureFileUploadReply(filename, srcAddress, dstAddress, servernumber, path, result, mode);
	file_upload_reply.setClientSenttime(clientSenttime);
	file_upload_reply.setServerRecvtime(serverRecvtime);
	file_upload_reply.setServerSenttime(serverSenttime);
	file_upload_reply.setClientRecvtime(clientRecvtime);
	rcv.enQRequest(file_upload_reply);
	return true;
};

//---------< handling server's file search request reply>----------------------
bool ClientHandler::fileSearchReplyHandling(Socket& socket_, Receiver& rcv){
	Message ms;
	std::string servernumber, numberoffiles, numberofdirectories, srcAddress, dstAddress, attribStr, asxml, patterns, serverrelativepath, mode, body;
	findFileSearchReplyAttributes(socket_, servernumber, numberoffiles, numberofdirectories, srcAddress, dstAddress, asxml, patterns, serverrelativepath, mode);

	body = socket_.recvString();

	// configure a "FILE_SEARCH_REPLY" message and enque into the client's receiver
	Message file_search_reply;
	file_search_reply.configureFileSearchReply(srcAddress, dstAddress, asxml, servernumber, numberoffiles, numberofdirectories, serverrelativepath, patterns, body, mode);
	rcv.enQRequest(file_search_reply);
	return true;
};

//---------< handling server's text search request reply>----------------------
bool ClientHandler::textSearchReplyHandling(Socket& socket_, Receiver& rcv){
	Message ms;
	std::string servernumber, numberoffiles, srcAddress, dstAddress, attribStr, asxml, patterns, searchtext, mode, body;

	// reverse the source address and destination address and collect all the other attributes
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "asxml"){
			asxml = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "mode"){
			mode = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "searchtext"){
			searchtext = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "patterns"){
			patterns = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "numberoffiles"){
			numberoffiles = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "servernumber"){
			servernumber = (attribStr.substr(attribStr.find(':') + 1));
		}
	}
	body = socket_.recvString();

	// configure a "FILE_SEARCH_REPLY" message and enque into the client's receiver
	Message text_search_reply;
	text_search_reply.configureTextSearchReply(srcAddress, dstAddress, asxml, servernumber, numberoffiles, patterns, searchtext, body, mode);
	rcv.enQRequest(text_search_reply);
	return true;
};

//----------------< private helper function for check whether the file is in processing >------------------------------
bool ClientHandler::openFileforProcessing(std::string& fullfilename, std::string& fileName, Socket& sc, std::ofstream& ofs){
	try{
		// if the file is currently not in processing, just create it, or if it already exists in the drive, override it
		if (FileSocketPairsInProcessing.count(fullfilename) == 0){
			ofs.open(fileName, std::ofstream::binary);
			FileSocketPairsInProcessing.insert({ fullfilename, &sc });
		}
		else if (FileSocketPairsInProcessing[fullfilename] == &sc){
			// if the file is currently in processing and is being written to by the socket_ itself, then append
			// the content to it untill the all the file blocks are received and written into the file                                                         
			ofs.open(fileName, std::ofstream::binary | std::ofstream::app);
		}
		else{
			// if the file is currently in processing and is being written to by another socket_ , then pause
			// and wait until the other socket_has finished writting
			std::unique_lock<std::mutex> l(mtx_);
			cv_.wait(l, [=](){ return FileSocketPairsInProcessing.count(fullfilename) == 0; });
			ofs.open(fileName, std::ofstream::binary);
		}
	}
	catch (std::exception& ex){
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n", always);
	}
	return true;
};

bool ClientHandler::writeToFileBufferData(bool& ok, char * buffer, int BufLen, Socket& sc, std::ofstream& ofs){
	if (sc.waitForData(100, 10))
	{
		ok = sc.recv(BufLen, buffer);
		if (sc == INVALID_SOCKET || !ok)
			return false;
		ofs.write(buffer, BufLen);
		Verbose::show("server recvd " + toString(BufLen) + " bytes");
	}
	else
	{
		Verbose::show("client wait for server timed-out", always);
	}
	return true;
}


//------< parameterized constructor, with port number, ip protocol, registration number and binded sender specified >------------
//------< for ex: if the receiver and a sender is owned by the same client/server, then the sender is binded to this receiver >--
//------< a thread is created that pulls requests from the sender blocking queue >-----------------------------------------------
Receiver::Receiver(size_t port, std::string loc, Socket::IpVer ipv, int qNum, Sender* s, bool isRecvThrdEnded) : loc_(loc), socketlstn(port, ipv), qNum_(qNum), s_(s), isReceiveThreadEnded(isRecvThrdEnded){
	socketlstn.start(ch, *this);
	std::thread t([this] { this->deQRequest(); });
	t.detach();
};

//----------< enque a message to the receiver queue >--------------------------------------------------
void Receiver::enQRequest(Message& ms){
	// for original test
	/*switch (qNum_){
	case 1:
	receiver1Q.get().enQ(ms);
	case 2:
	receiver2Q.get().enQ(ms);
	case 3:
	receiver3Q.get().enQ(ms);
	case 4:
	receiver4Q.get().enQ(ms);
	}*/
	receiverQ.enQ(ms);
};

//--------< deque a single message from the receiver queue and process it >-----------------------------
Message Receiver::deQSingleRequest(){
	return receiverQ.deQ();
};

//--------< deque a message at a time from the receiver queue and process it >---------------------------
//--------< it loops in a thread created in the receiver constructor >-----------------------------------
//--------< the loop breaks only when the it receives a quit reply message from the server>--------------
void Receiver::deQRequest(){
	while (!isReceiveThreadEnded){
		Message ms;
		ms = receiverQ.deQ();
		// switch condition used to dispatch different messages to different handlings
		switch (ms.getCommandNumberMap()[ms.getCommand()]){
		case 1:
			recvFileUploadDispath(ms);;
			break;
		case 2:
			recvFileUploadReplyDispath(ms);
			break;
		case 3:
			recvFileSearchDispath(ms);
			break;
		case 5:
			recvFileDownloadDispath(ms);
			break;
		case 6:
			recvFileDownloadReplyDispath(ms);
			break;
		case 7:
			recvTextSearchDispath(ms);
			break;
		}
	};
};


//-----------------< receiver dispatches for different messages >-------------------------------------
void Receiver::recvFileUploadDispath(Message& ms){
	std::string path = ms.getAttributes()["path"];
	Verbose::show(ms.getCommand() + " of " + ms.getFiletoUpload() + " has completed, the file stored under " + path + "\n", always);
	// add specific attributes for file upload reply
	ms.setCommand("FILE_UPLOAD_REPLY");
	ms.addAttrib("result", "success");
	ms.addAttrib("servernumber", toString(qNum_));
	ms.setServerSenttime(std::chrono::high_resolution_clock::now());
	s_->enQRequest(ms);
};

void Receiver::recvFileSearchDispath(Message& ms){
	std::string patterns, serverrelativepath, asxml;
	patterns = ms.getAttributes()["patterns"];
	serverrelativepath = ms.getAttributes()["serverrelativepath"];
	asxml = ms.getAttributes()["asxml"];

	// display the server's receipt of file download on server's screen
	Verbose::show("Has received " + ms.getCommand() + " of patterns " + patterns + " under path " + serverrelativepath + "\n", always);

	std::istringstream iss(patterns);
	std::vector<std::string> vpatterns{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };

	// process the file search request
	std::vector<std::string> files;
	int nodirectories = 0;
	FileCatalogue FC;
	FC.setPath(serverrelativepath);
	FC.setPatterns(vpatterns);
	FC.search(true);  // recursive search
	FC.getFiles(files, nodirectories);

	// add specific attributes for file search reply
	ms.setCommand("FILE_SEARCH_REPLY");
	ms.addAttrib("servernumber", getLoc().substr(0, 7));
	ms.addAttrib("numberoffiles", toString(files.size()));
	ms.addAttrib("numberofdirectories", toString(nodirectories));
	if (asxml == "False"){
		std::string filelist, machine = ms.getAttributes()["srcAddress"];
		for (auto file : files){
			/*filelist += machine + ' ' + file + '\n';*/
			filelist += file + '\n';
		}
		ms.setBody(filelist);
	}
	else{
		constructFileListXmlBody(ms, files);
	}
	s_->enQRequest(ms);
};

void Receiver::recvTextSearchDispath(Message& ms){
	std::string patterns, threads, asxml, searchtext;
	patterns = ms.getAttributes()["patterns"];
	threads = ms.getAttributes()["threads"];
	searchtext = ms.getAttributes()["searchtext"];
	size_t threadn = toOrginType<size_t>(threads);
	asxml = ms.getAttributes()["asxml"];

	// display the server's receipt of file download on server's screen
	Verbose::show("Has received " + ms.getCommand() + " of patterns " + patterns + " with " + threads + " threads specified \n", always);

	std::istringstream iss(patterns);
	std::vector<std::string> vpatterns{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };

	// process the file search request
	std::vector<std::string> files;
	int nodirectories = 0;
	FileCatalogue FC;
	FC.setPath(getLoc());
	FC.setPatterns(vpatterns);
	FC.search(true);  // recursive search
	FC.getFiles(files, nodirectories);

	// add specific attributes for file search reply
	ms.setCommand("TEXT_SEARCH_REPLY");
	ms.addAttrib("servernumber", getLoc().substr(0, 7));
	std::vector<std::string> resfiles;

	// single thread processing
	FC.multiThreadTextSearch(searchtext, files, 0, files.size() - 1, resfiles);

	ms.addAttrib("numberoffiles", toString(resfiles.size()));

	if (asxml == "False"){
		std::string filelist, machine = ms.getAttributes()["srcAddress"];
		for (auto file : resfiles){
			/*std::string file = Resfiles.deQ();*/
			filelist += machine + ' ' + file + '\n';
		}
		ms.setBody(filelist);
	}
	else{
		constructFileListXmlBody(ms, resfiles);
	}
	s_->enQRequest(ms);
}

void Receiver::constructFileListXmlBody(Message& ms, std::vector<std::string>& resfiles){
	XmlDocument textSearchResDoc;
	std::string roottext = "TextSearchResult: " + ms.getAttributes()["srcAddress"];
	textSearchResDoc.createRoot(makeTaggedElement(roottext));
	textSearchResDoc.element(roottext).addChild(makeTaggedElement("C++"));
	textSearchResDoc.element(roottext).addChild(makeTaggedElement("Java"));
	textSearchResDoc.element(roottext).addChild(makeTaggedElement("C#"));
	textSearchResDoc.element(roottext).addChild(makeTaggedElement("Other"));
	for (auto file : resfiles){
		/*std::string file = Resfiles.deQ();*/
		if (file.find(getLoc() + "./Java/") == 0 || file.find(getLoc() + "Java/") == 0){
			XmlDocument::sPtr newFile = makeTaggedElement("File");
			XmlDocument::sPtr newFilename = makeTextElement(file.substr(file.find("Java/") + 5));
			newFile->addChild(newFilename);
			textSearchResDoc.element("Java").addChild(newFile);
			textSearchResDoc.select();
		}
		else if (file.find(getLoc() + "./C#/") == 0 || file.find(getLoc() + "C#/") == 0){
			XmlDocument::sPtr newFile = makeTaggedElement("File");
			XmlDocument::sPtr newFilename = makeTextElement(file.substr(file.find("C#/") + 3));
			newFile->addChild(newFilename);
			textSearchResDoc.element("C#").addChild(newFile);
			textSearchResDoc.select();
		}
		else if (file.find(getLoc() + "./C++/") == 0 || file.find(getLoc() + "C++/") == 0){
			XmlDocument::sPtr newFile = makeTaggedElement("File");
			XmlDocument::sPtr newFilename = makeTextElement(file.substr(file.find("C++/")+4));
			newFile->addChild(newFilename);
			textSearchResDoc.element("C++").addChild(newFile);
			textSearchResDoc.select();
		}
		else{
			XmlDocument::sPtr newFile = makeTaggedElement("File");
			XmlDocument::sPtr newFilename = makeTextElement(file.substr(file.find("/") + 1));
			newFile->addChild(newFilename);
			textSearchResDoc.element("Other").addChild(newFile);
			textSearchResDoc.select();
		}
	}
	if (textSearchResDoc.element("C++").children().select().size() == 0)  textSearchResDoc.element(roottext).removeChild(textSearchResDoc.element("C++").select()[0]);
	if (textSearchResDoc.element("Java").children().select().size() == 0)  textSearchResDoc.element(roottext).removeChild(textSearchResDoc.element("Java").select()[0]);
	if (textSearchResDoc.element("C#").children().select().size() == 0)  textSearchResDoc.element(roottext).removeChild(textSearchResDoc.element("C#").select()[0]);
	if (textSearchResDoc.element("Other").children().select().size() == 0)  textSearchResDoc.element(roottext).removeChild(textSearchResDoc.element("Other").select()[0]);
	ms.setBody(textSearchResDoc.writeToXmlstring());
}

void Receiver::recvFileDownloadDispath(Message& ms){
	std::string serverrelativepath = ms.getAttributes()["serverrelativepath"];
	// display the server's receipt of file download on server's screen
	Verbose::show("Has received " + ms.getCommand() + " of " + ms.getFiletoUpload() + " under path " + serverrelativepath + "\n", always);
	// add specific attributes for file download reply
	ms.setCommand("FILE_DOWNLOAD_REPLY");
	ms.addAttrib("servernumber", getLoc().substr(0, 7));
	s_->enQRequest(ms);
};

void Receiver::recvFileUploadReplyDispath(Message& ms){
	std::string result, servernumber, path;
	result = ms.getAttributes()["result"];
	servernumber = ms.getAttributes()["servernumber"];
	path = ms.getAttributes()["path"];
	// display the server's reply of file upload on client's screen
	if (result == "success") Verbose::show(ms.getFiletoUpload() + " has been successfully uploaded to Server" + servernumber + " under " + path + "\n", always);
	else Verbose::show("The upload of " + ms.getFiletoUpload() + " to " + servernumber + " has failed.\n", always);
}

void Receiver::recvFileDownloadReplyDispath(Message& ms){
	std::string servernumber, serverrelativepath, localpath;
	servernumber = ms.getAttributes()["servernumber"];
	serverrelativepath = ms.getAttributes()["path"];
	localpath = ms.getAttributes()["localpath"];
	// display the server's reply of file upload on client's screen
	Verbose::show(ms.getFiletoUpload() + " from " + servernumber + " under " + serverrelativepath + " has been downloaded to local path " + localpath + "\n", always);
}
//--< get the registeration number for the client/server that is using the receiver >-----------------
int Receiver::getNum(){
	return qNum_;
}

// get the folder where the server/client resides
std::string Receiver::getLoc(){
	return loc_;
};

//----------< get the reference to the socket listener >----------------------------------------------
SocketListener& Receiver::getSocketListener(){
	return socketlstn;
};

//----------< check if the receiver thread state is valid >----------------------------
bool Receiver::receiveThreadStateValid(){
	return isReceiveThreadEnded == false;
}

Receiver::~Receiver(){
	//delete s_;
}

//-------------< test stub >------------------------------------------------
#ifdef TEST_RECEIVER

int main(){
	/*Sender s;*/
	Verbose v(true);
	SocketSystem ss;
	Receiver r(8090, "ReceiverFoler/", Socket::IP6, 1);
	std::cout << "\n  press key to exit: ";
	while (std::cin.get() != '\n')
	{
	}
}
#endif