/////////////////////////////////////////////////////////////////////////////
// ServerTest.cpp - Test passing strings, blocks, and buffers              //
// Ver 1.1                                                                 //
// Jim Fawcett, CSE#687 - Object Oriented Design, Spring 2015              //
// CST 4-187, Syracuse University, 315 443-3948, jfawcett@twcny.rr.com     //
//-------------------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                          //
// All rights granted provided this copyright notice is retained           //
//-------------------------------------------------------------------------//
// Application: Demo for Project #3, Communication Channel                 //
// Platform:    Dell 2720, Win Pro 8.1, Visual Studio 2013                 //
/////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package provides tests for Sockets-4.5:
*  - passing byte buffers
*  - passing strings
*
*  Required Files:
*  ---------------
*  ClientTest.cpp, ServerTest.cpp
*  Sockets.h, Sockets.cpp, AppHelpers.h, AppHelpers.cpp
*
*  Build Command:
*  --------------
*  devenv Sockets.sln /rebuild debug
*
*  Maintenance History:
*  --------------------
*  ver 1.1 : 30 Mar 15
*  - minor modification to comments, above
*  ver 1.0 : 30 Mar 15
*  - first release
*/
#include "../sockets/sockets.h"
#include "../ApplicationHelpers/AppHelpers.h"
#include <iostream>
#include <thread>
#include <mutex>

using namespace ApplicationHelpers;

/////////////////////////////////////////////////////////////////////////////
// Server's client handler class
// - must be callable object so we've built as a functor
// - passed to SocketListener::start(CallObject& co)
// - Client handling thread starts by calling operator()

class ClientHandler
{
public:
  void operator()(Socket& socket_);
  bool testStringHandling(Socket& socket_);
  bool testBufferHandling(Socket& socket_);
};

//----< Client Handler thread starts running this function >-----------------

void ClientHandler::operator()(Socket& socket_)
{
  while (true)
  {
    // interpret test command

    std::string command = socket_.recvString();
    if (command.size() == 0)
      break;
    Verbose::show("command: " + command);

    if (command == "TEST_STRING_HANDLING")
    {
      if (testStringHandling(socket_))
        Verbose::show("----String Handling test passed", always);
      else
        Verbose::show("----String Handling test failed", always);
      continue; // go back and get another command
    }
    if (command == "TEST_BUFFER_HANDLING")
    {
      if (testBufferHandling(socket_))
        Verbose::show("----Buffer Handling test passed", always);
      else
        Verbose::show("----Buffer Handling test failed", always);
      continue;  // get another command
    }
  }

  // we get here if command isn't requesting a test, e.g., "TEST_STOP"

  Verbose::show("ClientHandler socket connection closing");
  socket_.shutDown();
  socket_.close();
  Verbose::show("ClientHandler thread terminating");
}

//----< test string handling >-----------------------------------------------
/*
*   Creates strings, sends to server, then reads strings server echos back.
*/
bool ClientHandler::testStringHandling(Socket& socket_)
{
  title("String handling test on server");

  while (true)
  {
    if (socket_.bytesWaiting() == 0)
      break;
    std::string str = socket_.recvString();
    if (socket_ == INVALID_SOCKET)
      return false;
    if (str.size() > 0)
    {
      //Verbose::show("bytes recvd at server: " + toString(str.size() + 1));
      Verbose::show("server recvd : " + str, always);

      if (socket_.sendString(str))
      {
        //Verbose::show("bytes sent by server: " + toString(str.size() + 1));
      }
      else
      {
        return false;
      }
    }
    else
    {
      break;
    }
  }
  socket_.sendString("TEST_END");
  return true;
}
//----< test buffer handling >-----------------------------------------------
/*
*   Creates buffers, sends to server, then reads buffers server echos back.
*/
bool ClientHandler::testBufferHandling(Socket& socket_)
{
  title("Buffer handling test on server");
  const int BufLen = 20;
  Socket::byte buffer[BufLen];
  bool ok;

  while (true)
  {
    if (socket_.bytesWaiting() == 0)
      break;
    ok = socket_.recv(BufLen, buffer);
    if (socket_ == INVALID_SOCKET || !ok)
      return false;
    std::string temp;
    for (size_t i = 0; i < BufLen; ++i)
      temp += buffer[i];
    //Verbose::show("bytes recvd at server: " + toString(BufLen));
    Verbose::show("server recvd : " + temp, always);

    if (socket_.send(BufLen, buffer))
    {
      //Verbose::show("bytes sent by server: " + toString(BufLen));
    }
    else
    {
      return false;
    }
  }
  std::string temp = "TEST_END";
  for (size_t i = 0; i < BufLen; ++i)
  {
    if (i < temp.size())
      buffer[i] = temp[i];
    else
      buffer[i] = '#';
  }
  socket_.send(BufLen, buffer);
  return true;
}

//----< test stub starts here >----------------------------------------------

struct Cosmetic
{
  ~Cosmetic()
  {
    std::cout << "\n  press key to exit: ";
    std::cin.get();
    std::cout << "\n\n";
  }
} aGlobalForCosmeticAction;

//----< demonstration >------------------------------------------------------

int main()
{
  title("Testing Socket Server", '=');

  try
  {
    Verbose v(true);
    SocketSystem ss;
    SocketListener sl(9080, Socket::IP6);
    ClientHandler cp;
    sl.start(cp);

    Verbose::show("press key to exit: ", always);
    std::cout.flush();
    std::cin.get();
  }
  catch (std::exception& ex)
  {
    Verbose::show("  Exception caught:", always);
    Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
  }
}

