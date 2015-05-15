/////////////////////////////////////////////////////////////////////
// DataStore.cpp - Support file name and path storage                //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 1 - File Catalogue                  //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                             //
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "DataStore.h"
#include "..\DisplayHelper\DisplayHelper.h"
#include "..\FileCatalogue\FileSystem.h"

using namespace std;
using namespace DataStoreNS;
using namespace DisplayHelperNS;

// Saves the file, its path and the iterator to the path in the datastore map,
//  when full file specification is given
void DataStore::save(const string& fullfilespec){
	string filename = FileSystem::Path::getName(fullfilespec);
	string path = FileSystem::Path::getPath(fullfilespec);
	save(filename, path);
}

// Saves the path and saves the file and the iterator to the path in the map, 
//  used when the file doesn't exists or either path or iterator to the path
//   doesn't exist
void DataStore::save(const string& filename, const string& path){
	list<PathsIter> listOfIters = _store[filename];
	_paths.insert(path);
	listOfIters.push_back(_paths.find(path));
	_store[filename] = listOfIters;
}

// Gets the iterator for a specific file in the File-list<PathsIter> map
DataStore::iterator DataStore::findFile(const string& filename){
	DataStore::iterator it;
	for (it = _store.begin(); it != _store.end(); it++){
		if (it->first == filename) return it;
	}
	return it;
}

// Test if the iterator to a specifc path exists for a specific file in 
//  the File-list<PathsIter> map
bool DataStore::findPathIter(const string& filename, const string& path){
	list<PathsIter> lp = _store[filename];
	for (list<PathsIter>::iterator it = lp.begin(); it!=lp.end(); it++){
		if (**it == path) return true;
	}
	return false;
}

//----< test stub >--------------------------------------------------------
#ifdef TEST_DATASTORE
int main(){
	DisplayHelper DH;
	DataStore DS;
	DH.title("Test DataStore's save method with two parameters", '=');
	DS.save("test1.txt", "D:\\");
	DS.save("test1.txt", "D:\\FolderA");
	DS.save("test2.txt", "D:\\FolderA");
	for (auto ds : DS){
		DH.displayFilename(ds.first);
		for (auto p : ds.second){
			DH.displayPath(*p);
		}
		DH.displayInfo("");
	}

	DH.title("Test DataStore's save method with one parameter", '=');
	DS.save("D:\\newtest1.txt");
	DS.save("D:\\FolderA\\newtest2.txt");
	for (auto ds : DS){
		DH.displayFilename(ds.first);
		for (auto p : ds.second){
			DH.displayPath(*p);
		}
	}

	DH.title("Test DataStore's findFile method", '=');
	DataStore::iterator it = DS.findFile("test1.txt");
	DH.displayFilename(it->first);
	for (list<DataStore::PathsIter>::iterator lit = (it->second).begin(); lit != (it->second).end(); lit++)
		DH.displayPath(**lit);

	DH.title("Test DataStore's findPathIter method", '=');
	string filename = "test2.txt", path1 = "D:\\FolderA", path2 = "D:\\";
	string info1 = "The PathIter from " + filename + " to " + path1 + (DS.findPathIter(filename, path1) ? " exists" : " doesn't exist");
	DH.displayInfo(info1);
	string info2 = "The PathIter from " + filename + " to " + path2 + (DS.findPathIter(filename, path2) ? " exists" : " doesn't exist");
	DH.displayInfo(info2);
	DH.displayInfo("\n");
	return 0;
}

#endif