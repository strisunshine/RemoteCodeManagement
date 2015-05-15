#ifndef XMLTOTREE_H
#define XMLTOTREE_H
///////////////////////////////////////////////////////////////////////
// XmltoTree.h - Parse XML strings or files                          //
// ver 1.0                                                           //
// Language:    Visual C++, Visual Studio 2013                       //
// platform:    Windows 7                                            //
// Application: CSE687 2015 - Project 2 - XML Documnet Model         //
// Author:      Wenting Wang, Syracuse University, wwang34@syr.edu   //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains a single class, XmltoTree.  It provides a
* simple interface to parse a xml string or a xml file into an
* internal tree representation with the help from XmlElementParts
* package. It decides if the parts got is either XmlDeclarElement,
* ProcInstrElement, TaggedElement, TextElement and CommentElement.
* Definition for all the elements can be found in XmlElement
* package.
* 
* Public Interface:
* =================
* std::string Xmlstring; bool isFile;
* parseXML(Xmlstring);
*
* Required Files:
* ===============
* XmltoTree.h, XmltoTree.cpp, XmlElement.h, XmlElement.cpp, xmlElementParts.h, xmlElementParts.cpp, Tokenizer.h, Tokenizer.cpp
*
* Build Process:
* ==============
* devenv XmlDocument.sln /rebuild Debug
*
* Maintenance History:
* ====================
* ver 1.0 : 19 Mar 15
* - first release
*/

#include <memory>
#include <string>
#include "../XmlElement/XmlElement.h"
#include "../XmlElementParts/XmlElementParts/xmlElementParts.h"
#include "../XmlElementParts/XmlElementParts/Tokenizer.h"

namespace XmlProcessing{
	class XmltoTree{
	public:
		using sPtr = std::shared_ptr < AbstractXmlElement >;
		sPtr parseXML(const std::string& Xmlstring, bool isFile = true);
	private:
		bool addElement(XmlParts& parts, bool &isDecl, sPtr& currentEl);
		bool addDeclEl(XmlParts& parts, bool &isDecl, sPtr& currentEl);
		bool addProcInstrcEl(XmlParts& parts, bool &isDecl, sPtr& currentEl);
		bool addCommentEl(XmlParts& parts, sPtr& currentEl);
		bool addTaggedEl(XmlParts& parts, sPtr& currentEl);
		bool addTextEl(XmlParts& parts, sPtr& currentEl);
		bool encounterTaggedElEndAndGoUpper(XmlParts& parts, sPtr& currentEl);
		bool addSelfenclosingTaggedEl(XmlParts& parts, sPtr& currentEl);
	};
}

#endif