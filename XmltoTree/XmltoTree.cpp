///////////////////////////////////////////////////////////////////////
// XmltoTree.cpp - Parse XML strings or files                        //
// ver 1.0                                                           //
// Language:    Visual C++, Visual Studio 2013                       //
// platform:    Windows 7                                            //
// Application: CSE687 2015 - Project 2 - XML Documnet Model         //
// Author:      Wenting Wang, Syracuse University, wwang34@syr.edu   //
///////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include "XmltoTree.h"

using namespace std;
using namespace XmlProcessing;

//parse a xml string of file (indicated by isFile parameter), parse it into an internal xml element tree
shared_ptr<AbstractXmlElement> XmltoTree::parseXML(const string& Xmlstring, bool isFile){

	// make the DocElement
	sPtr docEl = makeDocElement();
	docEl->emptyChildren();
	Toker toker(Xmlstring, isFile);
	toker.setMode(Toker::xml);
	XmlParts parts(&toker);
	bool isDecl = true;
	sPtr currentEl = docEl;
	while (parts.get()){
		for (int i = 0; i < parts.length();i++){
			if (parts[i] == "*"){
				std::cout << "lala";
			}
		}
		if (addElement(parts, isDecl, currentEl)) continue;
		else std::cout << "\n invalid xml encountered" << "\n";
	}
	return docEl;
}

bool XmltoTree::addElement(XmlParts& parts, bool &isDecl, sPtr& currentEl){
	//if it is XmlDeclarElement, adds it and goes to get next "parts"
	if (addDeclEl(parts, isDecl, currentEl)){
		return true;
	}
	//if it is ProcInstrElement, adds it and goes to get next "parts"
	if (addProcInstrcEl(parts, isDecl, currentEl)){
		return true;
	}
	//if it is CommentElement, adds it and goes to get next "parts"
	if (addCommentEl(parts, currentEl)){
		return true;
	}
	//if it is TaggedElement, adds it and goes to get next "parts"
	if (addTaggedEl(parts, currentEl)){
		return true;
	}
	//if it is TextElement, adds it and goes to get next "parts"
	if (addTextEl(parts, currentEl)){
		return true;
	}
	//if it is TaggedElement ending and goes to upper level
	if (encounterTaggedElEndAndGoUpper(parts, currentEl)){
		return true;
	}
	//if it is self-enclosing TaggedElement, adds it and goes to get next "parts"
	if (addSelfenclosingTaggedEl(parts, currentEl)){
		return true;
	}
	return false;
}
//add XmlDeclarElement, return true if it's added
bool XmltoTree::addDeclEl(XmlParts& parts, bool &isDecl, sPtr& currentEl){
	if (parts.length() > 2 && parts[0] == "<" && parts[1] == "?"){
		sPtr DeclEl = nullptr;
		if (isDecl) {
			DeclEl = makeXmlDeclarElement(); isDecl = false;
		}
		else return false;
		bool nvpair = false;
		string name, value;
		for (int i = 3; i < parts.length() - 2; i++){
			if ((parts[i][0] == '\"' || parts[i][0] == '\'') && nvpair == true) {
				value = parts[i].substr(1, parts[i].length() - 2);
				DeclEl->addAttrib(name, value);
				nvpair = false;
			}
			else if (parts[i] == "=") continue;
			else{
				name = parts[i];
				nvpair = true;
			}
		}
		currentEl->addChild(DeclEl);
		return true;
	}
	return false;
}

//add ProcInstrElement, return true if it's added
bool XmltoTree::addProcInstrcEl(XmlParts& parts, bool &isDecl, sPtr& currentEl){
	if (parts.length() > 2 && parts[0] == "<" && parts[1] == "?"){
		sPtr ProcInstrcEl = nullptr;
		if (isDecl) return false;
		bool nvpair = false;
		string name, value, type;
		std::vector<std::string> typev;
		typev.push_back(parts[2]);
		name = parts[2];
		for (int i = 3; i < parts.length() - 2; i++){
			if ((parts[i][0] == '\"' || parts[i][0] == '\'') && nvpair == true) {
				typev.pop_back();
				if (!ProcInstrcEl){
					for (auto tp : typev) type += tp;
					ProcInstrcEl = makeProcInstrElement(type);
				}
				value = parts[i].substr(1, parts[i].length() - 2);
				ProcInstrcEl->addAttrib(name, value);
				nvpair = false;
			}
			else if (parts[i] == "=") continue;
			else{
				name = parts[i];
				typev.push_back(name);
				nvpair = true;
			}
		}
		currentEl->addChild(ProcInstrcEl);
		return true;
	}
	return false;
}


//add CommentElement, return true if it's added
bool XmltoTree::addCommentEl(XmlParts& parts, sPtr& currentEl){
	if (parts.length() > 2 && parts[0] == "<" && parts[1] == "!" && parts[2] == "--"){
		string comment;
		for (int i = 3; i < parts.length() - 2; i++){
			comment += parts[i];
			if (i != parts.length() - 3) comment += " ";
		}

		//if the comment cross more than one line
		if (parts[parts.length() - 1] != ">"){
			if (parts.length() > 3) { comment += " " + parts[parts.length() - 2] + " " + parts[parts.length() - 1] + " "; }
			while (parts.get()){
				if (parts[0] == "--" && parts[1] == ">") break;
				comment += parts.show();
			}
		}
		sPtr commEl = makeCommentElement(comment);
		currentEl->addChild(commEl);
		commEl->setParent(currentEl);
		return true;
	}
	return false;
}

//add TaggedElement, return true if it's added
bool XmltoTree::addTaggedEl(XmlParts& parts, sPtr& currentEl){
	if (parts.length() > 1 && parts[0] == "<" && parts[1] != "/" && parts[parts.length() - 2] != "/" && parts[parts.length() - 1] == ">"){
		string tag = parts[1];
		sPtr taggedEl = makeTaggedElement(tag);
		currentEl->addChild(taggedEl);
		taggedEl->setParent(currentEl);
		currentEl = taggedEl;
		bool nvpair = false;
		string name, value;
		for (int i = 2; i < parts.length() - 1; i++){
			if ((parts[i][0] == '\"' || parts[i][0] == '\'') && nvpair == true) {
				value = parts[i].substr(1, parts[i].length() - 2);
				currentEl->addAttrib(name, value);
				nvpair = false;
			}
			else if (parts[i] == "=") continue;
			else{
				name = parts[i];
				nvpair = true;
			}
		}
		return true;
	}
	return false;
}

//add TextElement, return true if it's added
bool XmltoTree::addTextEl(XmlParts& parts, sPtr& currentEl){
	if (parts.length() > 0 && parts[0] != "<" && parts[parts.length() - 1] != ">"){
		string text;
		for (int i = 0; i < parts.length(); i++){
			text += parts[i];
			if (i != parts.length() - 1) text += " ";
		}
		sPtr textEl = makeTextElement(text);
		currentEl->addChild(textEl);
		textEl->setParent(currentEl);
		return true;
	}
	return false;
}

//encounters TaggedElement ending and goes to upper level
bool XmltoTree::encounterTaggedElEndAndGoUpper(XmlParts& parts, sPtr& currentEl){
	if (parts.length() > 1 && parts[0] == "<" && parts[1] == "/" && parts[parts.length() - 1] == ">"){
		currentEl = currentEl->getParent();
		return true;
	}
	return false;
}

//add self-enclosing TaggedElement, return true if it's added
bool XmltoTree::addSelfenclosingTaggedEl(XmlParts& parts, sPtr& currentEl){
	if (parts.length() > 1 && parts[0] == "<" && parts[parts.length() - 2] == "/" && parts[parts.length() - 1] == ">"){
		string tag = parts[1];
		sPtr taggedEl = makeTaggedElement(tag);
		currentEl->addChild(taggedEl);
		taggedEl->setParent(currentEl);
		bool nvpair = false;
		string name, value;
		for (int i = 2; i < parts.length() - 2; i++){
			if ((parts[i][0] == '\"' || parts[i][0] == '\'') && nvpair == true) {
				value = parts[i].substr(1, parts[i].length() - 2);
				currentEl->addAttrib(name, value);
				nvpair = false;
			}
			else if (parts[i] == "=") continue;
			else{
				name = parts[i];
				nvpair = true;
			}
		}
		return true;
	}
	return false;
}



//-----< test stub >----------------------------------------------

#ifdef TEST_XMLTOTREE

int main(){

	//AbstractXmlElement::value();

	std::string Prologue = "<?xml version=\"1.0\" ?><!-- top level comment -->";
	std::string test1 = Prologue;
	test1 += "<parent att1='val1 with spaces' att2='val2'><child1 /><child2>child2 body<child1 /></child2></parent>";
	std::string test2 = Prologue;
	test2 += "<Dir><path><path1><path11>/TestFile/Dir1</path11></path1></path><File><name><name1>text1.txt</name1></name><date>02/11/2012</date></File></Dir>";

	std::cout << "\n  source = " << test1.c_str() << std::endl;
	XmltoTree XtoT;
	shared_ptr<AbstractXmlElement> docEl = XtoT.parseXML(test2, false);
    
	std::cout << "  " << docEl->toString();

	cout << "\n\n";

	shared_ptr<AbstractXmlElement> docEl2 = XtoT.parseXML("book.xml");

	std::cout << "  " << docEl2->toString();

	cout << "\n";

}

#endif