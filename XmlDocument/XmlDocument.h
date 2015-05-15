#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H
///////////////////////////////////////////////////////////////////
// XmlDocument.h - a container of XmlElement nodes               //
// Ver 1.3                                                       //
// Language:    Visual C++, Visual Studio 2013                   //
// platform:    Windows 7                                        //
// Application: CSE687 2015 - Project 2 - XML Documnet Model     //
// Author:      Wenting Wang, wwang34@syr.edu                    //
// Source:      Jim Fawcett, jfawcett@twcny.rr.com               //
///////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package is a XML Document Model.  It uses C++11 constructs,
* most noteably std::shared_ptr.  The XML Document Model is essentially
* a program-friendly wrapper around an Abstract Syntax Tree (AST) used to
* contain the results of parsing XML markup.
*
* Abstract Syntax Trees, defined in this package, are unordered trees with 
* two types of nodes:
*   Terminal nodes     - nodes with no children
*   Non-Terminal nodes - nodes which may have a finite number of children
* They are often used to contain the results of parsing some language.
*
* The elements defined in the companion package, XmlElement, will be used in
* the AST defined in this package.  They are:
*   AbstractXmlElement - base for all the XML Element types
*   DocElement         - XML element with children designed to hold prologue, Xml root, and epilogue
*   TaggedElement      - XML element with tag, attributes, child elements
*   TextElement        - XML element with only text, no markup
*   CommentElement     - XML element with comment markup and text
*   ProcInstrElement   - XML element with markup and attributes but no children
*   XmlDeclarElement   - XML declaration element with attributes
*
* Required Files:
* ---------------
*   - XmlDocument.h, XmlDocument.cpp, 
*     XmlElement.h, XmlElement.cpp
*
* Build Process:
* --------------
*   devenv XmlDocument.sln /rebuild Debug
*   
* Maintenance History:
* --------------------
* ver 1.3 : 19 Mar 15
* - added a few more functionalities, implemented all of the functionalities
* ver 1.2 : 21 Feb 15
* - modified these comments
* ver 1.1 : 14 Feb 15
* - minor changes to comments, method arguments
* Ver 1.0 : 11 Feb 15
* - first release 
*/

#include <memory>
#include <string>
#include "../XmlElement/XmlElement.h"
#include "../XmltoTree/XmltoTree.h"

namespace XmlProcessing
{
	struct IdValPair{
		std::string id;
		std::string value;
	};

  class XmlDocument
  {
  public:
    using sPtr = std::shared_ptr < AbstractXmlElement > ;

    enum sourceType { string, filename };
	XmlDocument() :pDocElement_(0){};  //default constructor
    XmlDocument(const std::string& src, sourceType srcType=string);
	XmlDocument(XmlDocument&& xd);
	XmlDocument& operator=(XmlDocument&& xd);
	XmlDocument(const XmlDocument& xd) = delete;
	XmlDocument& operator=(const XmlDocument& xd) = delete;
    XmlDocument& element(const std::string& tag = "");          
	XmlDocument& elementwID(const std::string& id, const std::string& idvalue);
    XmlDocument& elements(const std::string& tag = "");          
    XmlDocument& children(const std::string& tag = "");     
    XmlDocument& descendents(const std::string& tag = ""); 
	std::vector<std::pair<std::string, std::string>> attributes(const std::string& name = "");   
	bool addChild(sPtr pChild, sPtr pElement = 0); 
	bool removeChild(sPtr pChild, sPtr pElement = 0);  
	bool removeChild(const std::string& value, sPtr pElement = 0); 
	bool createRoot(sPtr pRoot); 
	bool addAttrib(const std::string& name, const std::string& value, sPtr pElement = 0);  
	bool removeAttrib(const std::string& name, sPtr pElement = 0); 
	std::vector<sPtr> select();                           
	bool writeToFile(std::string filename="");             
	std::string writeToXmlstring();          
	void findbyTag(sPtr& pNode, const std::string& tag)
	{
		if (tag == "" || pNode->tag() == tag) found_.push_back(pNode);
	}
	void findbyAttribID(sPtr& pNode, IdValPair& idval){
		for (auto attrib : pNode->getAttributes()){
			if (attrib.first == idval.id && attrib.second == idval.value){
				found_.push_back(pNode);
				return;
			}
		}
	}
  private:
    sPtr pDocElement_;         // AST that holds procInstr, comments, XML root, and more comments
    std::vector<sPtr> found_;  // query results
	
	
	template<typename Condition, typename CallObj>
	typename void BFS(sPtr& pElement, Condition& cd, CallObj& co);
  };

  template<typename Condition, typename CallObj>
  typename void XmlDocument::BFS(sPtr& pElement, Condition& cd, CallObj& co){
	  if (!pElement) return;
	  std::vector<sPtr> v, tempv;
	  v.push_back(pElement);
	  while (!v.empty()){
		  for (size_t i = 0; i < v.size(); i++){
			  for (auto ch : v[i]->getChildren()){
				  tempv.push_back(ch);
				  co(this, ch, cd);
			  }
		  }
		  v = tempv;
		  tempv.clear();
	  }
  }
}


#endif
