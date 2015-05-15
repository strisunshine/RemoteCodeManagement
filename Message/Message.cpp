/////////////////////////////////////////////////////////////////////
// Message.cpp - Support message construction and configuration    //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 3 - Message Passing            //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////

#include "Message.h"

// constructor
Message::Message(){
	commandNumberMap.insert({ "FILE_UPLOAD", 1 });
	commandNumberMap.insert({ "FILE_UPLOAD_REPLY", 2 });
	commandNumberMap.insert({ "FILE_SEARCH", 3 });
	commandNumberMap.insert({ "FILE_SEARCH_REPLY", 4 });
	commandNumberMap.insert({ "FILE_DOWNLOAD", 5 });
	commandNumberMap.insert({ "FILE_DOWNLOAD_REPLY", 6 });
	commandNumberMap.insert({ "TEXT_SEARCH", 7 });
	commandNumberMap.insert({ "TEXT_SEARCH_REPLY", 8 });
	commandNumberMap.insert({ "QUIT", 9 });
	commandNumberMap.insert({ "QUIT_REPLY", 10 });
}
// get the command-number map
std::map<std::string, int> Message::getCommandNumberMap(){
	return commandNumberMap;
};
// set the command
bool Message::setCommand(const std::string& command){
	command_ = command;
	return true;
};
// get the command
std::string Message::getCommand(){
	return command_;
};

//set the body
bool Message::setBody(const std::string& body){
	body_ = body;
	return true;
};

// get the body
std::string Message::getBody(){
	return body_;
};

// get the name of the file to be uploaded
std::string Message::getFiletoUpload(){
	return filename_;
};

// set the name of the file to be uploaded
bool Message::setFiletoUpload(const std::string& filename){
	filename_ = filename;
	return true;
};

// get the block length when send the file block by block
size_t Message::getFileUploadBlockLength(){
	return fileUploadBlockLen_;
};

// build a message for file upload used by the client
void Message::configureFileUploadMsg(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& path, const std::string& mode, size_t contentLen){
	clear();
	command_ = "FILE_UPLOAD";
	filename_ = filename;
	fileUploadBlockLen_ = contentLen;
	clientSenttime = std::chrono::high_resolution_clock::now();
	attribs_.insert({ "path", path });
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "mode", mode });
}

// build a message for file search used by client
void Message::configureFileSearchMsg(const std::string& srcAddress, const std::string& dstAddress, const std::string& asxml, const std::string& patterns, const std::string& serverrelativepath, const std::string& mode){
	clear();
	command_ = "FILE_SEARCH";
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "asxml", asxml});
	attribs_.insert({ "patterns", patterns });
	attribs_.insert({ "serverrelativepath", serverrelativepath });
	attribs_.insert({ "mode", mode });
}

// build a message for text search used by client
void Message::configureTextSearchMsg(const std::string& srcAddress, const std::string& dstAddress, const std::string& asxml, const std::string& patterns, const std::string& searchtext, const std::string& threads, const std::string& mode){
	clear();
	command_ = "TEXT_SEARCH";
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "asxml", asxml });
	attribs_.insert({ "patterns", patterns });
	attribs_.insert({ "searchtext", searchtext });
	attribs_.insert({ "threads", threads });
	attribs_.insert({ "mode", mode });
}

// build a message for file download used by the client
void Message::configureFileDownloadMsg(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, std::string& serverrelativepath, const std::string& mode, size_t contentLen){
	clear();
	command_ = "FILE_DOWNLOAD";
	filename_ = filename;
	fileUploadBlockLen_ = contentLen;
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "path", serverrelativepath });
	attribs_.insert({ "mode", mode });
}

// build a message for quitting used by the client
void Message::configureQuitMsg(const std::string srcAddress, const std::string dstAddress){
	clear();
	command_ = "QUIT";
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
}

// recover a message received for file upload reply used by the client
void Message::configureFileUploadReply(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& servernumber, std::string path, const std::string& result, const std::string& mode){
	clear();
	setFiletoUpload(filename);
	setCommand("FILE_UPLOAD_REPLY");
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "mode", mode });
	attribs_.insert({ "result", result });
	attribs_.insert({ "servernumber", servernumber });
	attribs_.insert({ "path", path});
};

// recover a message received for file search reply used by the client
void Message::configureFileSearchReply(const std::string& srcAddress, const std::string& dstAddress, const std::string& asxml, const std::string& servernumber, const std::string& numberoffiles, const std::string& numberofdirectories, const std::string& serverrelativepath, const std::string& patterns, const std::string& body, const std::string& mode){
	clear();
	setCommand("FILE_SEARCH_REPLY");
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "asxml", asxml });
	attribs_.insert({ "mode", mode });
	attribs_.insert({ "numberoffiles", numberoffiles });
	attribs_.insert({ "numberofdirectories", numberofdirectories });
	attribs_.insert({ "servernumber", servernumber });
	attribs_.insert({ "serverrelativepath", serverrelativepath });
	attribs_.insert({ "patterns", patterns });
	setBody(body);
};

// recover a message received for text search reply used by the client
void Message::configureTextSearchReply(const std::string& srcAddress, const std::string& dstAddress, const std::string& asxml, const std::string& servernumber, const std::string& numberoffiles, const std::string& patterns, const std::string& searchtext, const std::string& body, const std::string& mode){
	clear();
	setCommand("TEXT_SEARCH_REPLY");
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "asxml", asxml });
	attribs_.insert({ "mode", mode });
	attribs_.insert({ "numberoffiles", numberoffiles });
	attribs_.insert({ "servernumber", servernumber });
	attribs_.insert({ "searchtext", searchtext });
	attribs_.insert({ "patterns", patterns });
	setBody(body);
};

// build a message for file download reply used by the server
void Message::configureFileDownloadReply(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& servernumber, std::string& serverrelativepath, std::string& localpath, const std::string& mode, size_t contentLen){
	clear();
	setFiletoUpload(filename);
	setCommand("FILE_DOWNLOAD_REPLY");
	fileUploadBlockLen_ = contentLen;
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "mode", mode });
	attribs_.insert({ "servernumber", servernumber });
	attribs_.insert({ "serverrelativepath", serverrelativepath });
	attribs_.insert({ "localpath", localpath });
};

// get the block length when send the file block by block
void Message::setFileUploadBlockLength(size_t contentLen){
	fileUploadBlockLen_ = contentLen;
}
// add attribute to the message
bool Message::addAttrib(const std::string& name, const std::string& value){
	std::pair<std::string, std::string> attrib = { name, value };
	attribs_.insert(attrib);
	return true;
};
// get all the attributes of the message
std::map<std::string, std::string> Message::getAttributes(){
	return attribs_;
};
// clear the files of the message that have been set previously
void Message::clear(){
	command_ = "";
	filename_ = "";
	fileUploadBlockLen_ = 0;
	attribs_.clear();
};


#ifdef TEST_MESSAGE

int main()
{
	Message ms;
	ms.configureFileUploadMsg("SameFile.txt", "::1@9080", "::1@8090");
	ms.configureQuitMsg("::1@9080", "::1@8090");
	ms.configureFileUploadReply("SameFile.txt", "::1@9080", "::1@8090", "1", "../Server", "success");
	return 0;
}


#endif