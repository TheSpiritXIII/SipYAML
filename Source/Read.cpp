#include <iostream>
#include "SipYAML.hpp"
using std::cout;
using std::endl;

const char *source =
	"---\n"
	"#This is a comment.\n"
	"FirstKey   	  : YAY!!!#My first key!\n"
	"  Child : I'm baby :3\n"
	"Second		: Oh no...\n";

int main()
{
	Sip::YAMLDocumentUTF8 doc1;
	doc1.parse(source);
	cout << "Iterating through keys: " << endl;
	Sip::YAMLDocumentUTF8::Node *node = doc1.firstChild();
	while (node)
	{
		cout << "Map Node: " << node->keySize() << endl;
		if (node->key())
		{
			cout << "  Key: \"";
			cout.write(node->key(), node->keySize());
			cout << "\"" << endl;
		}
		if (node->value())
		{
			cout << "  Value: \"";
			cout.write(node->value(), node->valueSize());
			cout << "\"" << endl;
		}
		node = node->nextSibling();
	}
	return 0;
}