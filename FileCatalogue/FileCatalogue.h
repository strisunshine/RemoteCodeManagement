#ifndef FILECATALOGUE_H
#define FILECATALOGUE_H

/////////////////////////////////////////////////////////////////////
// FileCatalogue.h - Support file catalogue operations             //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 1 - File Catalogue             //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                             
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains one class, FileCatalogue.
* 
* The FileCatalogue class support file searches. The resulted files will
* be saved in data store with the help of the DataStore class. The 
* FileCatalogue class support showing a brief summary, showing duplicate
* files and file content search after the catalogue construction. 
*
* Public Interface:
* =================
* FileCatalogue FC;
* FC.search(true);
* FC.summary();
* FC.showDuplicate();
* FC.textSearch("search text", {"*.*"});
*
* Required Files:
* ===============
* FileSystem.h, FileSystem.cpp, DataStore.h, DataStore.cpp, DisplayHelper.h, DisplayHelper.cpp
*
* Maintenance History:
* ====================
* Version 1.0 : 9 Feb 2015
* - first release
*/

#include <string>
#include <vector>
#include <list>
#include "FileSystem.h"
#include "../DataStore/DataStore.h"

namespace FileCatalogueNS
{
	class FileCatalogue{
	public:
		// FileCalogue constructor taking path and patterns
		FileCatalogue(std::string path = ".", std::vector<std::string> patterns = { "*.*" })
			: _path(path), _patterns(patterns){};

		// searches files according to the path and patterns provided, 
		//  if the bool parameter is true, recursive search will be
		//   done, results stored in a DataStroe object
		void search(bool isSwitchPresent = false);

		// shows a brief summary of the file catalogue 
		void summary();

		// get the files and directories from the datastore 
		void getFiles(std::vector<std::string>& files, int& nodirectories);

		// shows files with the same name but reside on different paths
		void showDuplicate();
		
		// searches text in the constructed file catalogue, while searchpatterns define the search scope
		void textSearch(const std::string& text, const std::vector<std::string>& searchpatterns);

		// searches text in the constructed file catalogue, while searchpatterns define the search scope
		void multiThreadTextSearch(const std::string& text, const std::vector<std::string>& files, int start, int end, std::vector<std::string>& resfiles);

		// sets the value of _patterns
		void setPatterns(const std::vector<std::string>& patterns){ _patterns = patterns; };

		// sets the value of _path
		void setPath(const std::string& path){ _path = path; };

		// finds specific text in a specified file
		bool findText(const std::string& text, const std::string& fullfilespec);
	private:
		// finds the files in a specified directory
		void find(const std::string& fullfilepath);
		std::string _path;
		std::vector<std::string> _patterns;
		DataStoreNS::DataStore _datastore;
	};
}









#endif