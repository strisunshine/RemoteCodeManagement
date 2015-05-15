/////////////////////////////////////////////////////////////////////
// DisplayHelper.cpp - Utilities for File Catalogue demos          //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 1 - File Catalogue             //  
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////

#include "DisplayHelper.h"
using namespace std;
using namespace DisplayHelperNS;

// Display title
void DisplayHelper::title(const string& title, char ornament, bool first){
	if (!first) cout << "\n";
	cout << "\n  " + title + "\n " + string(title.size() + 2, ornament);
};

// Display file name
void DisplayHelper::displayFilename(string file){
	cout << "\n File: " << file;
}

// Display directory
void DisplayHelper::displayPath(string path){
	cout << "\n Found in path: " << path;
}

// Display full file path
void DisplayHelper::displayFullpath(string fullpath){
	cout << "\n Text was found in: " << fullpath;
}

// Display a summary of the file catalogue
void DisplayHelper::displaySummary(int filecount, int directorycount){
	cout << "\n " << filecount << " Files found in " << directorycount << " directories";
}

// Dispaly regular information
void DisplayHelper::displayInfo(string s){
	cout << "\n " << s;
}

//----< test stub >--------------------------------------------------------
#ifdef TEST_DISPLAYHELPER
int main(){
	DisplayHelper DH;
	DH.title("Test DisplayHelper class", '=');
	DH.title("Test displaying title", '*');
	DH.displayFilename("TestFilename.txt");
	DH.displayPath("TestDrive:\\TestParentPath\\TestPath");
	DH.displayFullpath("TestDrive:\\TestParentPath\\TestPath\\TestFilename.txt");
	DH.displayInfo("Test displaying regular information");
	DH.displayInfo("\n");
	return 0;
}
#endif