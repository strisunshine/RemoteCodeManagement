#ifndef DataStore_H
#define DataStore_H

/////////////////////////////////////////////////////////////////////
// DataStore.h - Support file name and path storage                //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 1 - File Catalogue                  //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                             //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains one class, DataStore.
*
* The DataStore class support storage of file names as well as storage of
* a list of references of paths where the files are found. If file, its
* corresponding path and their connection are already in the data store, 
* no further saving will be done. Otherwise, either the filename or its 
* connection to the path will be saved.
*
* Public Interface:
* =================
* DataStore DS;
* DS.save("D:\\test.txt");
* DS.save("test.txt", "."); 
* DataStore::iterator it = DS.findfile("test.txt");
* bool findpathiter = DS.findPathIter("test.txt", ".");
*
* Maintenance History:
* ====================
* Version 1.0 : 9 Feb 2015
* - first release
*/

#include <string>
#include <set>
#include <map>
#include <list>

namespace DataStoreNS{
	class DataStore
	{
	public:
		using Path = std::string;
		using Paths = std::set<Path>;
		using PathsIter = std::set<Path>::iterator;
		using File = std::string;
		
		// Keep storage of file names as well as a list of references into the path set
		//  where they are found
		using Store = std::map<File, std::list<PathsIter>>;

		using iterator = Store::iterator;
		iterator begin(){ return _store.begin(); };
		iterator end(){ return _store.end(); };

		// Saves the file, its path and the iterator to the path in the datastore map,
		//  when full file specification is given
		void save(const std::string& fullfilespec);

		// Saves the path and saves the file and the iterator to the path in the map, 
		//  used when the file doesn't exists or either path or iterator to the path
		//   doesn't exist
		void save(const std::string& filename, const std::string& path);

		// Gets the iterator for a specific file in the File-list<PathsIter> map
		iterator findFile(const std::string& filename);

		// Test if the iterator to a specifc path exists for a specific file in 
		//  the File-list<PathsIter> map
		bool findPathIter(const std::string& filename, const std::string& path);

		// Return the set of paths in the data store
		Paths getPaths(){ return _paths; };
	private:
		Paths _paths;
		Store _store;
	};
}






#endif