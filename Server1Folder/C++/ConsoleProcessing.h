#ifndef CONSOLEPROCESSING_H
#define CONSOLEPROCESSING_H
///////////////////////////////////////////////////////////////
// ConsoleProcessing.h - Support for Console Applications    //
// ver 1.0                                                   //
// Application: Demonstrate language design for Console      //
// Language:    C++11, Visual Studio 2013                    //
// Platform:    Win 8.1 Pro, Dell 2720 - iCore7              //
// Author:      Jim Fawcett, Syracuse University, CST 4-187  //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2014 //
///////////////////////////////////////////////////////////////
/*
   Package Operations:
   ===================
   Provides support for parsing command line, showing environment,
   showing files on specified path that match specified patterns,
   and detecting if the application is running from Visual Studio.

   Maintanence Info:
   =================

   Required Files:
   ---------------
     ConsoleProcessing.h, ConsoleProcessing.cpp
     FileSystem.h, FileSystem.cpp

   Build Command:
   --------------
     devenv democurrentdirectory.sln /rebuild debug

   Maintanence History:
   --------------------
     ver 1.0 - 03 Feb 2014
     - first release
*/
#include <string>
#include <vector>
#include <tuple>

class ConsoleProcessing
{
public:
  using Files = std::vector<std::string>;
  using Path = std::string;
  using Patterns = std::vector<std::string>;
  using Options = std::vector<std::string>;
  using CommandLine = std::tuple<Path, Patterns, Options>;

  static std::string Title(const std::string& title, char underline = '=');
  static void showEnvironment();
  static bool IsRunningInVisualStudio();
  static void showCommandLine(int argc, char* argv[]);
  static Path currentPath();
  static void showFiles(const Path& path, Patterns& patts);
  static bool IsOption(const std::string& token);
  static CommandLine parseCommandLine(int argc, char* argv[]);
  static void showCommandLineParse(int argc, char* argv[]);
};

#endif
