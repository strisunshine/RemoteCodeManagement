/////////////////////////////////////////////////////////////////////////////
// ClientTest.cpp - Test passing strings, blocks, and buffers              //
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

using namespace ApplicationHelpers;

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

//----< test string handling - server echos back client sent string >--------

void clientTestStringHandling(Socket& si)
{
  si.sendString("TEST_STRING_HANDLING");

  for (size_t i = 0; i < 5; ++i)
  {
    std::string text = "Hello World " + std::string("#") + toString(i + 1);
    si.sendString(text);
    Verbose::show("client sending string: " + text, always);
  }

  if (si.waitForData(100, 10))
  {
    while (true)
    {
      std::string str = si.recvString();
      if (str.size() == 0 || str == "TEST_END")
        break;
      Verbose::show("client recvd: " + str, always);
    }
  }
  else
  {
    Verbose::show("client wait for server timed-out", always);
  }
}
//----< test buffer handling - server echos back client sent buffer >--------

void clientTestBufferHandling(Socket& si)
{
  si.sendString("TEST_BUFFER_HANDLING");
  const int BufLen = 20;
  Socket::byte buffer[BufLen];

  for (size_t i = 0; i < 5; ++i)
  {
    std::string text = "Hello World " + std::string("#") + toString(i + 1);
    for (size_t i = 0; i < BufLen; ++i)
    {
      if (i < text.size())
        buffer[i] = text[i];
      else
        buffer[i] = '#';
    }
    buffer[BufLen - 1] = '\0';
    si.send(BufLen, buffer);
    Verbose::show("client sending buffer: " + std::string(buffer), always);
  }

  if (si.waitForData(100, 10))
  {
    while (true)
    {
      for (size_t i = 0; i < BufLen; ++i)
        buffer[i] = '\0';
      si.recv(BufLen, buffer);
      std::string str(buffer);
      if (str.size() == 0 || str.find("TEST_END") < std::string::npos)
        break;
      Verbose::show("client recvd: " + str, always);
    }
  }
  else
  {
    Verbose::show("client wait for server timed-out", always);
  }
}
//----< demonstration >------------------------------------------------------

int main()
{
  title("Testing Socket Client", '=');

  try
  {
    Verbose v(true);
    SocketSystem ss;
    SocketConnecter si;
    while (!si.connect("localhost", 9080))
    {
      Verbose::show("client waiting to connect");
      ::Sleep(100);
    }

    title("Starting string test on client");
    clientTestStringHandling(si);
    title("Starting buffer test on client");
    clientTestBufferHandling(si);

    si.sendString("TEST_STOP");

    Verbose::show("\n  client calling send shutdown\n");
    si.shutDownSend();
  }
  catch (std::exception& ex)
  {
    Verbose::show("  Exception caught:", always);
    Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
  }
}
