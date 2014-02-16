#include "SipYAML.hpp"
#include <fstream>
using std::ofstream;

#include <iostream>
using std::cout;
using std::endl;

int main()
{
	Sip::YAMLDocumentUTF8 document;
	
	// NOTE: sizeof() - 1 to avoid null pointer.
	Sip::YAMLDocumentUTF8::Node *preprocessor = document.allocateNode(Sip::Directive);
	preprocessor->setKey("YAML", sizeof("YAML") - 1);
	preprocessor->setValue("1.2", sizeof("1.2") - 1);
	document.appendNode(preprocessor);
	
	Sip::YAMLDocumentUTF8::Node *docBeginNode = document.allocateNode(Sip::DocBegin);
	document.appendNode(docBeginNode);
	
	Sip::YAMLDocumentUTF8::Node *invokeNode = document.allocateNode(Sip::MapElement);
	invokeNode->setKey("invoice", sizeof("invoice") - 1);
	invokeNode->setValue("34843");
	document.appendNode(invokeNode);
	
	Sip::YAMLDocumentUTF8::Node *billToNode = document.allocateNode(Sip::MapElement);
	billToNode->setKey("bill-to", sizeof("bill-to") - 1);
	document.appendNode(billToNode);
	
	Sip::YAMLDocumentUTF8::Node *numberNode = document.allocateNode(Sip::ListElement);
	numberNode->setValue("300");
	billToNode->appendNode(numberNode);
	
	std::string printer;
	document.print(&printer);
	ofstream file("Output.txt");
	file << printer.data();
	
	//Sip::YAMLDocument<short> doc2;
	
	return 0;
}