/////////////////////////////////////////////////////////////////////
// Parser.cpp - Support command line and console input parsing     //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 1 - File Catalogue             //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      // 
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "..\FileCatalogue\FileCatalogue.h"
#include "..\DisplayHelper\DisplayHelper.h"
#include "Parser.h"

using namespace std;
using namespace FileCatalogueNS;
using namespace ParserNS;
using namespace DisplayHelperNS;


// read from command line all the parameters and iterate through 
//  each parameter to get all the information the FileCatalogue object
//   need for file catalogue construction and other operations
void Parser::readCommandline(int argc, char* argv[]){

	if (argc > 1){
		_path = argv[1];
		string prevs;
		for (int i = 2; i < argc; i++){
			string s = argv[i];
			transform(s.begin(), s.end(), s.begin(), ::tolower);
			if (s == "/s") _isswitchpresent = true;
			else if (s == "/d") _displayduplicate = true;
			else if (s == "/f") {
				_issearchpresent = true;
				prevs = s;
			}
			else if (s.find("/f") == 0) {
				_issearchpresent = true;
				_texttosearch = s.substr(2, s.length() - 1);	
			}
			else if (s.find(".") != string::npos && s.find(".") != 0 && s.find(".") != s.size() - 1)  _patterns.push_back(s);
			else if (prevs == "/f") _texttosearch = s;
			else continue;
		}
	}
}

// read from console the text to be searched and file patterns to be  
//  searched in. Then the processing is directed called on the FileCatalogue
//   object referenced in the argument.
void Parser::readFromConsoleandProecess(FileCatalogue& FC){
	DisplayHelper DH;
	DH.title("Read From Console", '=');
	while (true){
		DH.title("Please provide search text (placed in quotes) and file patterns you would like to narrow down the search (Press Enter to exit):", '-');
		DH.displayInfo("");
		_searchpatterns.clear();
		_texttosearch.clear();
		string s = ""; 
		getline(cin, s);
		if (s == "") break;
		int pos = 0, poe = 0, qcount = 0;  //start and end position of the quotes, aka the text to be searched
		for (size_t i = 0; i < s.length(); i++){
			if (s[i] == '\"'){
				qcount++;
				if (qcount == 1) pos = i;
				else poe = i;
			}
		}
		if(qcount>0) _texttosearch = s.substr(pos + 1, poe-pos-1);
		s.erase(pos, poe-pos+1);
		istringstream iss(s);
		string subs;	
		while (iss >> subs){ _searchpatterns.push_back(subs); };
		_issearchpresent = true;
		doProcessing(FC, true);
	}
}

// processing is called on the FileCatalogue object referenced in the 
//  argument, with all the needed flags and information supplied by 
//   the Parse class private data.
void Parser::doProcessing(FileCatalogue& FC, bool isconsole){
	if (!isconsole){
		FC.setPath(_path);
		FC.setPatterns(_patterns);
		FC.search(_isswitchpresent);
		FC.summary();
		if (_displayduplicate) FC.showDuplicate();
	}
	if (_issearchpresent) FC.textSearch(_texttosearch, _searchpatterns);
}

//----< test stub >--------------------------------------------------------
#ifdef TEST_PARSER
int main(){
	DisplayHelper DH;
	Parser P;
	FileCatalogue FC;
	DH.title("Test Parser's readCommandline method");
	DH.displayInfo("");
	int argc = 6; 
	char* argv[] = { "jjxx", "..", "*.*", "/d", "/s", "/f\"main\"" };
	P.readCommandline(argc, argv);
	DH.title("Test Parser's doProcessing method");
	P.doProcessing(FC);
	DH.title("Test Parser's readFromConsoleandProecess method");
	P.readFromConsoleandProecess(FC);

}
#endif