#ifndef DISPLAYHELPER_H
#define DISPLAYHELPER_H
/////////////////////////////////////////////////////////////////////
// DisplayHelper.cpp - Utilities for Xml Document Demo             //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 2 - Xml Documnet Model         //  
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains one class, DisplayHelper.
*
* The DisplayHelper class provide functions to display result prompt,
* element query results and attribute query results related to Xml
* Document Model demos.
*
* Public Interface:
* =================
* DisplayHelper::displayResultPrompt();
* DisplayHelper::displayElementQueryResult(vector of elements, if isrecurisively display each element)
* DisplayHelper::displayAttributesQueryResult();
*
* Maintenance History:
* ====================
* Version 1.0 : 9 Feb 2015
* - first release
*/

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include "../XmlElement/XmlElement.h"
#include "../XmlDocument/XmlDocument.h"

namespace DisplayHelperNS{
	class DisplayHelper{
	public:
		// Display a section
		static void section(const std::string& title, char decoChar = '*');

		// Display prompt for result
		static void displayResultPrompt();

		// Display query result that contains elements
		static void displayElementQueryResult(std::vector<std::shared_ptr<XmlProcessing::AbstractXmlElement>> v);

		// Display query result that contains attributes
		static void displayAttributesQueryResult(std::vector<std::pair<std::string, std::string>> v);
	};
}


#endif