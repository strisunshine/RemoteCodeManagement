#ifndef DISPLAYHELPER_H
#define DISPLAYHELPER_H

/////////////////////////////////////////////////////////////////////
// DisplayHelper.h - Utilities for File Catalogue demos            //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 1 - File Catalogue             //  
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains one class, DisplayHelper.
*
* The DisplayHelper class provide functions to display titles, files,
* directories, full paths and regular information related to File 
* Catalogue demos.
*
* Public Interface:
* =================
* DisplayHelper DH;
* DH.title("title");
* DH.displayFilename("test.txt");
* DH.displayPath("D:\CSE687\Project1");
* DH.displayFullpath("D:\CSE687\Project1\FileCatalogueDemo.exe");
* DH.displayInfo("");
* int filecount = 5, directorycount = 11;
* DH.displaySummary(filecount, directorycount);
*
* Maintenance History:
* ====================
* Version 1.0 : 9 Feb 2015
* - first release
*/

#include <iostream>
#include <string>
#include <list>

namespace DisplayHelperNS{
	class DisplayHelper{
	public:
		// Display title
		void title(const std::string& title, char ornament = '*', bool first = false);

		// Display file name
		void displayFilename(std::string file);

		// Display directory
		void displayPath(std::string path);

		// Display full file path
		void displayFullpath(std::string fullpath);

		// Display a summary of the file catalogue
		void displayInfo(std::string s);

		// Dispaly regular information
		void displaySummary(int filecount, int directorycount);
	};
}


#endif