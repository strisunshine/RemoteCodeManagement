#ifndef MESSAGE_H
#define MESSAGE_H

/////////////////////////////////////////////////////////////////////
// Message.h - Support message construction and configuration      //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 3 - Message Passing            //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains one class, Message.
*
* The Message class support buiding different categories of messages. 
* These messages have certain fields as well as a few pairs of 
* attribute name and value. It also contains methods to build some
* static categories of messages that some fileds at set with default
* values. 
*
* Public Interface:
* =================
* Message ms;
* ms.configureFileUploadMsg("test.txt", "::1@9080", "::1@8090");
* ms.configureQuitMsg("::1@9080", "::1@6070");
* ms.setCommand("Quit")
* ms.setFiletoUpload("test.txt")
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

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>

//#include "../ApplicationHelpers/AppHelpers.h"
//using namespace ApplicationHelpers;

class Message{
public:
	// constructor 
	Message();
	// set the command
	bool setCommand(const std::string& command);
	// get the command
	std::string getCommand();
	// get the command-number map
	std::map<std::string, int> getCommandNumberMap();
	// set the body
	bool setBody(const std::string& body);
	// get the body
	std::string getBody();
	// get the name of the file to be uploaded
	std::string getFiletoUpload();
	// set the name of the file to be uploaded
	bool setFiletoUpload(const std::string& filename);
	// get the block length when send the file block by block
	size_t getFileUploadBlockLength();
	// build a message for file upload used by client
	void configureFileUploadMsg(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& path = "", const std::string& mode = "oneway", size_t contentLen = 1024);
	// build a message for file search used by client
	void configureFileSearchMsg(const std::string& srcAddress, const std::string& dstAddress, const std::string& asxml, const std::string& patterns, const std::string& serverrelativepath = ".", const std::string& mode = "oneway");
	// build a message for text search used by client
	void Message::configureTextSearchMsg(const std::string& srcAddress, const std::string& dstAddress, const std::string& asxml, const std::string& patterns, const std::string& searchtext, const std::string& threads = "1", const std::string& mode = "oneway");
	// build a message for file download used by client
	void configureFileDownloadMsg(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, std::string& serverrelativepath, const std::string& mode = "oneway", size_t contentLen = 1024);
	// build a message for quitting used by client
	void configureQuitMsg(const std::string srcAddress, const std::string dstAddress);
	// recover a message received for file upload reply used by the client
	void configureFileUploadReply(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& servernumber, std::string path, const std::string& result, const std::string& mode = "oneway");
	// recover a message received for file upload reply used by the client
	void configureFileSearchReply(const std::string& srcAddress, const std::string& dstAddress, const std::string& asxml, const std::string& servernumber, const std::string& numberoffiles, const std::string& numberofdirectories, const std::string& serverrelativepath, const std::string& patterns, const std::string& body, const std::string& mode);
	// recover a message received for file search reply used by the client
	void Message::configureTextSearchReply(const std::string& srcAddress, const std::string& dstAddress, const std::string& asxml, const std::string& servernumber, const std::string& numberoffiles, const std::string& patterns, const std::string& searchtext, const std::string& body, const std::string& mode = "oneway");
	// build a message for file download reply used by the server
	void configureFileDownloadReply(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& servernumber, std::string& serverrelativepath, std::string& localpath, const std::string& mode = "oneway", size_t contentLen = 1024);
	// get the block length when send the file block by block
	void setFileUploadBlockLength(size_t contentLen);
	// add attribute to the message
	bool addAttrib(const std::string& name, const std::string& value);
	// get all the attributes of the message
	std::map<std::string, std::string> getAttributes();
	// clear the files of the message that have been set previously
	void clear();
	void setClientSenttime(std::chrono::time_point<std::chrono::high_resolution_clock> clientSenttimep){ clientSenttime = clientSenttimep; };
	void setServerRecvtime(std::chrono::time_point<std::chrono::high_resolution_clock> serverRecvtimep){ serverRecvtime = serverRecvtimep; };
	void setServerSenttime(std::chrono::time_point<std::chrono::high_resolution_clock> serverSenttimep){ serverSenttime = serverSenttimep; };
	void setClientRecvtime(std::chrono::time_point<std::chrono::high_resolution_clock> clientRecvtimep){ clientRecvtime = clientRecvtimep; };
	std::chrono::time_point<std::chrono::high_resolution_clock> getClientSenttime(){ return clientSenttime; };
	std::chrono::time_point<std::chrono::high_resolution_clock> getServerRecvtime(){ return serverRecvtime; };
	std::chrono::time_point<std::chrono::high_resolution_clock> getServerSenttime(){ return serverSenttime; };
	std::chrono::time_point<std::chrono::high_resolution_clock> getClientRecvtime(){ return clientRecvtime; };
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> clientSenttime;
	std::chrono::time_point<std::chrono::high_resolution_clock> serverRecvtime;
	std::chrono::time_point<std::chrono::high_resolution_clock> serverSenttime;
	std::chrono::time_point<std::chrono::high_resolution_clock> clientRecvtime;
	std::map<std::string, int> commandNumberMap;
	std::string command_;
	std::string filename_;
	std::string body_;
	size_t fileUploadBlockLen_;
	//used a vectored at first, need to discuss pros and cos yet
	/*std::vector<std::pair<std::string, std::string>> attribs_;*/ 
	std::map<std::string, std::string> attribs_;
};











#endif