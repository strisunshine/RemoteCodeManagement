///////////////////////////////////////////////////////////////////////////
// Server.cpp - Used for testing concurrent communication, representing //
//             one of the servers that can receive and process messages  // 
//             sent by different clients concurrently and send back      //
//             replies to the clients                                    //          
//                                                                       //
// ver 1.0                                                               //                                                    
// Application: CSE687 2015 Project 3 - Message Passing                  //
// Platform:    Win7, Visual Studio 2013                                 //
// Author:      Wenting Wang, wwang34@syr.edu                            //                                            
///////////////////////////////////////////////////////////////////////////

#include "Server.h"

using namespace ApplicationHelpers;

#ifdef TEST_SERVER

int main(int argc, char* argv[]){

	try{
		Verbose v(false);
		SocketSystem ss;
		size_t port = 8090;
		std::string loc;
		// set up the port for the server
		if (argc > 1) port = toOrginType<size_t>(argv[1]);
		if (argc > 2) loc = argv[2];
		Verbose::show("Starting Server (port: " + toString(port) + ", loc: " + loc + ") \n", always);
		Server server(port, loc);

		while (1);
	}
	catch (std::exception& ex)
	{
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
	}
	return 0;
}

#endif