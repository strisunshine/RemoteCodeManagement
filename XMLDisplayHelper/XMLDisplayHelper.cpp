/////////////////////////////////////////////////////////////////////
// DisplayHelper.cpp - Utilities for Xml Document Demo             //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 2 - Xml Documnet Model         //  
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////

#include <iomanip>
#include "XMLDisplayHelper.h"
using namespace std;
using namespace DisplayHelperNS;
using namespace XmlProcessing;

// Display a section
void DisplayHelper::section(const std::string& title, char decoChar)
{
	int number = 20;
	std::cout << "\n " << std::string(number, decoChar) << " " << title << " " << std::string(number, decoChar);
}

// Display prompt for result
void DisplayHelper::displayResultPrompt(){
	cout << "\n The result is as following: \n";
}

// Display query result that contains elements
void DisplayHelper::displayElementQueryResult(std::vector<std::shared_ptr<AbstractXmlElement>> v){
	if (v.size() == 0){
		cout << "\n The element(s) doesn't exist" << "\n";
		return;
	}
	for (auto el : v){
		cout << el->toString() << '\n';
	}
}

// Display query result that contains attributes
void DisplayHelper::displayAttributesQueryResult(std::vector<std::pair<std::string, std::string>> v){
	if (v.size() == 0){
		cout << "\n The attribute(s) dosen't exist";
		return;
	}
	cout << "\n " << setw(10) << left << "name" << setw(20) << left << "value" << "\n";
	for (size_t i = 0; i < v.size(); i++){
		cout << " " << setw(10) << left << v[i].first << setw(20) << left << v[i].second << '\n';
	}
}
//----< test stub >--------------------------------------------------------
#ifdef TEST_DISPLAYHELPER
int main(){
	using sPtr = std::shared_ptr < AbstractXmlElement >;

	//Create Xml Elements
	sPtr root = makeTaggedElement("root");
	root->addChild(makeTextElement("this is a test"));
	sPtr child = makeTaggedElement("child");
	child->addChild(makeTextElement("this is another test"));
	child->addAttrib("first", "testone");
	child->addAttrib("second", "testtwo");
	root->addChild(child);
	sPtr docEl = makeDocElement(root);

	DisplayHelper::section("Test displayResultPrompt()");
	DisplayHelper::displayResultPrompt();
	DisplayHelper::section("Test displayElementQueryResult()");
	DisplayHelper::displayElementQueryResult(root->getChildren());
	DisplayHelper::section("Test displayAttributesQueryResult()");
	DisplayHelper::displayAttributesQueryResult(child->getAttributes());
	return 0;
}
#endif