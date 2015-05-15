#ifndef PARSER_H
#define PARSER_H

/////////////////////////////////////////////////////////////////////
// Parser.h - Support command line and console input parsing       //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 1 - File Catalogue             //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      // 
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains one class, Parser.
*
* The Parser class support command line input and console input parsing,
* the parsing result is recorded as the Parser class prive data and will
* be passed to the member functions of the FileCatalogue class. The 
* processing is directly done in the Parser class by calling a FileCatalogue
* object as an argument.
*
* Public Interface:
* =================
* Parse P;
* P.readCommandline(int argc, char* argv[]);
* P.readFromConsoleandProecess(FileCatalogueNS::FileCatalogue& FC);
* P.doProcessing(FileCatalogueNS::FileCatalogue& FC);
*
* Required Files:
* ===============
* DataStore.h, DataStore.cpp, FileSystem.h, FileSystem.cpp, FileCatalogue.h, FileCatalogue.cpp, DisplayHelper.h, DisplayHelper.cpp
*
* Maintenance History:
* ====================
* Version 1.0 : 9 Feb 2015
* - first release
*/

#include <vector>

namespace ParserNS{
	class Parser{
	public:
		// read from command line all the parameters and iterate through 
		//  each parameter to get all the information the FileCatalogue object
		//   need for file catalogue construction and other operations
		void readCommandline(int argc, char* argv[]);

		// read from console the text to be searched and file patterns to be  
		//  searched in. Then the processing is directed called on the FileCatalogue
		//   object referenced in the argument.
		void readFromConsoleandProecess(FileCatalogueNS::FileCatalogue& FC);
		
		// processing is called on the FileCatalogue object referenced in the 
		//  argument, with all the needed flags and information supplied by 
		//   the Parse class private data.
		void doProcessing(FileCatalogueNS::FileCatalogue& FC, bool isconsole = false);

	private:
		std::string _path;
		bool _isswitchpresent = false;
		bool _displayduplicate = false;
		bool _issearchpresent = false;
		std::string _texttosearch = "";
		std::vector<std::string> _patterns;
		std::vector<std::string> _searchpatterns;
	};
}

#endif