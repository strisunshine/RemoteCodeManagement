///////////////////////////////////////////////////////////////////
// XmlDocument.cpp - define XML Element types                     //
// ver 1.4                                                       //
// Language:    Visual C++, Visual Studio 2013                   //
// platform:    Windows 7                                        //
// Author:      Wenting Wang, wwang34@syr.edu                    //
// Adapted from: Jim Fawcett, jfawcett@twcny.rr.com              //
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <ctime>
#include <cstdlib>
#include "XmlDocument.h"
#include "../XMLDisplayHelper/XMLDisplayHelper.h"

using namespace XmlProcessing;
using namespace DisplayHelperNS;

// parameterized constructor 
XmlDocument::XmlDocument(const std::string& src, sourceType srcType)
{
	XmltoTree XtoT;
	if (srcType == 0) pDocElement_ = XtoT.parseXML(src, false);
	else pDocElement_ = XtoT.parseXML(src, true);
}

// move constructor
XmlDocument::XmlDocument(XmlDocument&& xd) : pDocElement_(xd.pDocElement_), found_(xd.found_){
	xd.pDocElement_ = nullptr;
}

// move assignment operator
XmlDocument& XmlDocument::operator=(XmlDocument&& xd){
	if (this == &xd) return *this;
	pDocElement_ = xd.pDocElement_;
	found_ = xd.found_;
	xd.pDocElement_ = nullptr;
	return *this;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// queries return XmlDocument references so they can be chained, e.g., doc.element("foobar").descendents();

// found_[0] contains first element (BFS order) with tag
XmlDocument& XmlDocument::element(const std::string& tag){
	sPtr res;
	elements(tag);
	if (found_.size() > 0){
		res = found_[0];
		found_.clear();
		found_.push_back(res);
	}
	else found_.clear();
	return *this;
}

// found_ contains all elements with tag
XmlDocument& XmlDocument::elements(const std::string& tag){
	sPtr curr;
	if (found_.size() > 0){
		curr = found_[0];
		found_.clear();
	}
	else curr = pDocElement_;
	if (!curr) return *this;
	found_.clear();
	findbyTag(curr, tag);
	BFS(curr, tag, std::mem_fn(&XmlDocument::findbyTag));
	return *this;
}

// found_ contains sPtr to the elements with the unique id value
XmlDocument& XmlDocument::elementwID(const std::string& id, const std::string& idvalue){
	IdValPair iv = { id, idvalue };
	sPtr curr;
	if (found_.size() > 0){
		curr = found_[0];
		found_.clear();
	}
	else curr = pDocElement_;
	if (!curr) return *this;
	found_.clear();
	findbyAttribID(curr, iv);
	BFS(curr, iv, std::mem_fn(&XmlDocument::findbyAttribID));
	return *this;
};

// found_ contains sPtrs to children of prior found_[0] 
XmlDocument& XmlDocument::children(const std::string& tag){
	std::vector<sPtr> v;
	if (found_.size() > 0){
		if (tag == "") v = found_[0]->getChildren();
		else{
			for (auto ch : found_[0]->getChildren()){
				if (tag == ch->tag()){
					v.push_back(ch);
				}
			}
		}
	}
	else if (pDocElement_) v = pDocElement_->getChildren();
	found_.clear();
	found_ = v;
	return *this;
}

// attributes_found contains all the name-value attribute pairs of prior found_[0]
std::vector<std::pair<std::string, std::string>> XmlDocument::attributes(const std::string& name){
	std::vector<std::pair<std::string, std::string>> attributes_found;
	if (found_.size() > 0){
		if (name == "") attributes_found = found_[0]->getAttributes();
		else{
			std::vector<std::pair<std::string, std::string>> attributes;
			attributes = found_[0]->getAttributes();
			for (size_t i = 0; i < attributes.size(); i++)
			{
				if (attributes[i].first == name){
					attributes_found.push_back(attributes[i]);
				}
			}
		}
	}
	return attributes_found;
};

// found_ contains sPtrs to descendents of prior found_[0]
XmlDocument& XmlDocument::descendents(const std::string& tag){
	sPtr curr;
	if (found_.size() > 0){
		curr = found_[0];
		found_.clear();
	}
	else curr = pDocElement_;
	if (!curr) return *this;
	found_.clear();
	BFS(curr, tag, std::mem_fn(&XmlDocument::findbyTag));
	return *this;
}

//add child by sPtr to the parent, when used in a chain, the parent is prior found_[0]
bool XmlDocument::addChild(std::shared_ptr < AbstractXmlElement > pChild, std::shared_ptr < AbstractXmlElement > pElement){
	if (pElement == 0){
		if (found_.size() > 0) return found_[0]->addChild(pChild);
	}
	else return pElement->addChild(pChild);
	return false;
}

//remove child by pointer to the parent, when used in a chain, the parent is prior found_[0]
bool XmlDocument::removeChild(std::shared_ptr < AbstractXmlElement > pChild, std::shared_ptr < AbstractXmlElement > pElement){
	if (pElement != 0)  return pElement->removeChild(pChild);
	if (found_.size() > 0)  return found_[0]->removeChild(pChild);
	return false;
};

//add child by value to the parent, when used in a chain, the parent is prior found_[0]
bool  XmlDocument::removeChild(const std::string& value, std::shared_ptr < AbstractXmlElement > pElement){
	if (pElement != 0)  return pElement->removeChild(value);
	if (found_.size() > 0)  return found_[0]->removeChild(value);
	return false;
};

//add a root element to an empty document tree
bool XmlDocument::createRoot(sPtr pRoot){
	if (!pDocElement_){
		pDocElement_ = makeDocElement(pRoot);
		return true;
	}
	else if (pDocElement_->getChildren().size() == 0){
		sPtr pDecl = makeXmlDeclarElement();
		pDecl->addAttrib("version", "1.0");
		pDocElement_->addChild(pDecl);
		if (pRoot != nullptr){
			pDocElement_->addChild(pRoot);
			return true;
		}
	}
	return false;
};

//add an attribute pair to the parent, when used in a chain, the parent is prior found_[0]
bool XmlDocument::addAttrib(const std::string& name, const std::string& value, sPtr pElement){
	if(pElement!=0) return pElement->addAttrib(name, value);
	else if (found_.size() > 0) return found_[0]->addAttrib(name, value);
	return false;
};

//remove an attribute to the parent, when used in a chain, the parent is prior found_[0]
bool XmlDocument::removeAttrib(const std::string& name, sPtr pElement){
	if (pElement != 0)  return pElement->removeAttrib(name);
	if (found_.size() > 0)  return found_[0]->removeAttrib(name);
	return false;
};

// return reference to found_.  Use std::move(found_) to clear found_
std::vector<XmlDocument::sPtr> XmlDocument::select(){
	std::vector<XmlDocument::sPtr> res = std::move(found_);
	return res;
}

// write the internal xml tree to a xml file
bool XmlDocument::writeToFile(std::string filename){
	std::ofstream xmlfile;
	std::string s;
	if (pDocElement_) s = pDocElement_->toString();
	if (filename == ""){
		std::stringstream ss;
		srand((int)time(0));
		int i = (int)rand() % 100000;
		ss << i;
		ss >> filename;
		filename = "GeneratedFile" + filename + ".xml";
	}
	else{
		if (filename.length()<4 || filename.substr(filename.length() - 4, filename.length()) != ".xml") filename += ".xml";
	}
	xmlfile.open(filename);
	xmlfile << s << '\n';
	xmlfile.close();
	return true;
}

// return a xml string represents thae internal tree
std::string XmlDocument::writeToXmlstring(){
	std::string s;
	if (pDocElement_){
		s = pDocElement_->toString();		
	}
	else std::cout << "\n  The doc contains no element.";
	return s;
}

//-----< test stub >----------------------------------------------

#ifdef TEST_XMLDOCUMENT

int main()
{
  title("Testing XmlDocument class");

  title("\n Testing building tree representation from xml files");
  XmlDocument doc1("book.xml", XmlDocument::sourceType::filename);
  std::cout << "\n\n doc1: \n" << doc1.writeToXmlstring();
  XmlDocument doc2("LectureNote.xml", XmlDocument::sourceType::filename);
  std::cout << "\n\n doc2: \n" << doc2.writeToXmlstring();
  title("\n Testing building tree representation from xml strings");
  std::string Prologue = "<?xml version=\"1.0\" ?><!-- top level comment -->";
  std::string test1 = Prologue;
  test1 += "<parent att1='val1 with spaces' att2='val2'><child1 /><child2>child2 body<child1 /></child2></parent>";
  std::string test2 = Prologue;
  test2 += "<Dir><path><path1><path11>/TestFile/Dir1</path11></path1></path><File><name><name1>text1.txt</name1></name><date>02/11/2012</date></File></Dir>";
  XmlDocument doc3(test1, XmlDocument::sourceType::string);
  std::cout << "\n\n doc3: \n" << doc3.writeToXmlstring();
  XmlDocument doc4(test2, XmlDocument::sourceType::string);
  std::cout << "\n\n doc4: \n" << doc4.writeToXmlstring();

  title("\n Testing elements(\"title\") ");
  doc2.elements("title"); 
  DisplayHelper::displayElementQueryResult(doc2.select());

  title("\n Testing element(\"reference\") ");
  doc2.element("reference");
  DisplayHelper::displayElementQueryResult(doc2.select());

  title("\n Testing element(\"reference\").descendents() ");
  doc2.element("reference").descendents();
  DisplayHelper::displayElementQueryResult(doc2.select());

  title("\n Testing element(\"reference\").children() ");
  doc2.element("reference").children();
  DisplayHelper::displayElementQueryResult(doc2.select());

  title("\n Testing attributes() ");
  std::vector<std::pair<std::string, std::string>> ares = doc2.element("note").attributes();
  doc2.select();
  DisplayHelper::displayAttributesQueryResult(ares);
  ares = doc2.element("LectureNote").attributes();
  doc2.select();
  DisplayHelper::displayAttributesQueryResult(ares);
  
  title("\n Testing elements(\"book\") ");
  doc1.elements("book");
  DisplayHelper::displayElementQueryResult(doc1.select());

  title("\n Testing element(\"book\").attributes()");
  std::vector<std::pair<std::string, std::string>> v1 = doc1.element("book").attributes();
  doc1.select();
  DisplayHelper::displayAttributesQueryResult(v1);


  title("\n Testing elementwID(\"id\", \"bk103\") ");
  doc1.elementwID("id", "bk103");
  DisplayHelper::displayElementQueryResult(doc1.select());

  title("\n Testing elementwID(\"id\", \"bk100\") ");
  doc1.elementwID("id", "bk100");
  DisplayHelper::displayElementQueryResult(doc1.select());

  title("\n Testing addChild(makeTaggedElement(\"country\")) ");
  if (doc1.elementwID("id", "bk101").addChild(makeTaggedElement("country"))){
	  std::cout << "\n added the child" << "\n";
  };
  title("\n Testing addChild(makeTextElement(\"USA\")) ");
  if (doc1.elementwID("id", "bk101").element("country").addChild(makeTextElement("USA"))){
	  std::cout << "\n added the grand child" << "\n";
  };

  title("\n Testing addAttrib(\"lang\", \"Eng\") ");
  if(doc1.elementwID("id", "bk101").element("country").addAttrib("lang", "Eng")){
	  std::cout << "\n added the attribute" << "\n";
  };
  std::cout << doc1.writeToXmlstring();

  title("\n Testing removeChild(smartpointer)");
  doc1.select();
  XmlDocument::sPtr temEl;
  std::vector<XmlDocument::sPtr> v2;
  v2 = doc1.elementwID("id", "bk103").select();
  if (v2.size() > 0){
	  temEl = v2[0];
  };
  if (doc1.element("catalog").removeChild(temEl)){
	  std::cout << "\n removed the child with id value \"bk103\"";
	  std::cout << doc1.writeToXmlstring();
  }

  title("\n Testing removeAttrib(\"lang\")");
  doc1.select();
  if (doc1.elementwID("id", "bk101").element("country").removeAttrib("lang")){
	  std::cout << "\n removed the attribute with name \"lang\" of the book element with id value \"bk101\"";
	  std::cout << doc1.writeToXmlstring();
  };

  title("\n Testing removeChild(\"country\")");
  doc1.select();
  if (doc1.elementwID("id", "bk101").removeChild("country")){
	  std::cout << "\n removed the child of the element with id value \"bk101\"";
	  std::cout << doc1.writeToXmlstring();
  };

  title("\n Testing createRoot(\"RootElement\")");
  XmlDocument doc5;
  if (doc5.createRoot(makeTaggedElement("RootElement"))){
	  std::cout << "\n Root element was created.";
  };
  std::cout << doc5.writeToXmlstring();
  
  title("\n Testing addChild() on created root");
  v2 = doc5.elements().select();
  if(v2.size()>2) temEl = v2[2];
  if (temEl->addChild(makeTaggedElement("FirstChild"))){
	  std::cout << "\n Child was added to the root element.";
  };
  std::cout << doc5.writeToXmlstring();

  title("\n Testing move constrution, move construct doc6 from doc2");
  XmlDocument doc6(std::move(doc2));
  std::cout << "\n doc6 is as following:" << std::endl;
  std::cout << doc6.writeToXmlstring() << "\n";
  std::cout << "\n doc2 is as following:" << std::endl;
  std::cout << doc2.writeToXmlstring() << "\n";

  title("\n Testing move assignment operation, move assign doc3 to doc6");
  doc6 = std::move(doc3);
  std::cout << "\n doc6 is as following:" << std::endl;
  std::cout << doc6.writeToXmlstring() << "\n";
  std::cout << "\n doc3 is as following:" << std::endl;
  std::cout << doc3.writeToXmlstring();

  title("\n Testing writing the tree representation to a xml file");
  if (doc1.writeToFile("NewFilel")){
	  std::cout << "\n doc1 was sucessfully written to NewFile1.xml";
  };
  if(doc2.writeToFile("NewFile2")){
	  std::cout << "\n doc2 was sucessfully written to NewFile1.xml";
  };
  if(doc3.writeToFile()){
	  std::cout << "\n doc3 was sucessfully written to an unamed new file";
  };
  if(doc4.writeToFile()){
	  std::cout << "\n doc4 was sucessfully written to an unamed new file";
  };
  std::cout << "\n\n";
}

#endif