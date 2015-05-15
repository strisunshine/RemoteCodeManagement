/////////////////////////////////////////////////////////////////////
// FileCatalogue.cpp - Support file catalogue operations           //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 1 - File Catalogue             //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////

#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <regex>
#include "FileSystem.h"
#include "FileCatalogue.h"
#include "..\DisplayHelper\DisplayHelper.h"

using namespace std;
using namespace DataStoreNS;
using namespace FileCatalogueNS;
using namespace DisplayHelperNS;



// searches files according to the path and patterns provided, 
//  if the bool parameter is true, recursive search will be
//   done, results stored in a DataStroe object
void FileCatalogue::search(bool isSwtchPresent){
	string path = _path;
	if (isSwtchPresent){
		find(path);
		vector<string> dirs = FileSystem::Directory::getDirectories(path);
		if(dirs.size()>0) dirs.erase(dirs.begin(), dirs.begin() + 2);
		for (auto& dir : dirs){
			dir = FileSystem::Path::fileSpec(path, dir);
		}
		while (dirs.size()!=0){
			vector<string> tempdirs;
			for (auto dir : dirs){
				find(dir);
				vector<string> subdirs = FileSystem::Directory::getDirectories(dir);
				if(subdirs.size()>0) subdirs.erase(subdirs.begin(), subdirs.begin() + 2);
				for (size_t i = 0; i < subdirs.size(); i++){
					subdirs[i] = FileSystem::Path::fileSpec(dir, subdirs[i]);
				}
				tempdirs.insert(tempdirs.end(), subdirs.begin(), subdirs.end());
			}
			dirs = tempdirs;
		}
	}
	else{
		find(_path);
	}
}

// finds the files in a specified directory
void FileCatalogue::find(const string& path){
	string fullpath = FileSystem::Path::getFullFileSpec(path);
	for (auto patt : _patterns){
		vector<string> files = FileSystem::Directory::getFiles(path, patt);
		for (auto f : files){
			DataStore::iterator it = _datastore.findFile(f);
			if (it == _datastore.end() || (it != _datastore.end() && !_datastore.findPathIter(f, path)))
				_datastore.save(f, path);
		}
	}
}

// shows a brief summary of the file catalogue 
void FileCatalogue::summary(){
	DisplayHelper DH;
	DH.title("Summary", '-');
	int filecount = 0, directorycount = 0;
	for (auto d : _datastore){
		for (auto p : d.second){
			filecount++;
		}
	}
	for (auto p : _datastore.getPaths()){
		directorycount++;
	}
	DH.displaySummary(filecount, directorycount);
}

// get the files and number of directories from the datastore 
void FileCatalogue::getFiles(std::vector<std::string>& files, int& nodirectories){
	int directorycount = 0;
	for (auto d : _datastore){
		for (auto p : d.second){
			files.push_back(FileSystem::Path::fileSpec(*p, d.first));
		}
	}
	for (auto p : _datastore.getPaths()){
		directorycount++;
	};
	nodirectories = directorycount;
}

// shows files with the same name but reside on different paths
void FileCatalogue::showDuplicate(){
	DisplayHelper DH;
	DH.title("Duplicate files:", '-');
	for (DataStore::iterator it = _datastore.begin(); it != _datastore.end(); it++){
		if ((it->second).size() >= 2) {
			DataStore::File df = it->first;
			DH.displayFilename(df);
			DataStore::iterator it = _datastore.findFile(df);
			if (it != _datastore.end()){
				for (auto pathpointer : it->second){
					DH.displayPath(*pathpointer);
				}
			}
		}
	}
}

// searches text in the constructed file catalogue, while searchpatterns define the search scope
void FileCatalogue::textSearch(const string& text, const vector<string>& searchpatterns){
	DisplayHelper DH;
	string s = "Search Results of \"" + text + "\" : ";
	DH.title(s, '-');
	for (DataStore::iterator it = _datastore.begin(); it != _datastore.end(); it++){
		DataStore::File filename = it->first;
		bool isfilematchpattern = false;
		if (searchpatterns.size() > 0){
			for (auto searchpattern : searchpatterns){
				string fext = FileSystem::Path::getExt(filename);
				string fname = FileSystem::Path::getName(filename, false);
				string pext = FileSystem::Path::getExt(searchpattern);
				string pname = FileSystem::Path::getName(searchpattern, false);
				if ((pext == "*" || fext == pext) && (pname == "*" || fname == pname)){
					isfilematchpattern = true; break;
				}
			}
			if (!isfilematchpattern) continue;
		}
		for (auto pathpointer : it->second){
			string path = *pathpointer;
			string fullfilespec = FileSystem::Path::fileSpec(path, filename);
			findText(text, fullfilespec);
		}
	}
}

// searches text in the constructed file catalogue, while searchpatterns define the search scope
void FileCatalogue::multiThreadTextSearch(const string& text, const vector<string>& files, int start, int end, vector<string>& resfiles){
	for (int i = start; i <= end; i++){
		if (findText(text, files[i])) resfiles.push_back(files[i]);
	}
	
}

// finds specific text in a specified file
bool FileCatalogue::findText(const string& text, const std::string& fullfilespec){
	FileSystem::File fi(fullfilespec);
	fi.open(FileSystem::File::in);
	if (fi.isGood()){
		string content = fi.readAll(false);
		transform(content.begin(), content.end(), content.begin(), ::tolower);
		string transtext = text;
		transform(text.begin(), text.end(), transtext.begin(), ::tolower);
		if (content.find(transtext) != string::npos){
			//DH.displayFullpath(fullfilespec);
			return true;
		}
	}
	fi.close();
	return false;
}

//----< test stub >--------------------------------------------------------
#ifdef TEST_FILECATALOGUE
int main()
{
	DisplayHelper DH;
	DH.title("Test FileCatalogue's methods", '=');
	FileCatalogue FC;
	FC.setPath(".."); 
	FC.setPatterns({ "*.*" });
	FC.search(true);
	FC.summary();
	FC.showDuplicate();
	FC.textSearch("main", { "*.h", "*.cpp" });
	return 0;
}

#endif