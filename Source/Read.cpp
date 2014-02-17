#include <iostream>
#include "SipYAML.hpp"
using std::cout;
using std::endl;

const char *source =
	"---\n"
	"#This is a comment.\n"
	"FirstKey:    	  YAY!!!#My first key!\n"
	"  Child : I'm baby :3\n"
	"Second: 		Oh no...\n";

void indent(size_t amount)
{
	for (size_t i = 0; i != amount; ++i)
	{
		cout << '\t';
	}
}

void showYAMLValue(Sip::YAMLDocumentUTF8::Node *node, size_t amount = 0)
{
	while (node)
	{
		indent(amount);
		cout << "Node: ";
		switch (node->type() & 0xF)
		{
		case Sip::Sequence:
			cout << "Sequence";
			break;
		case Sip::Mapping:
			cout << "Mapping";
			break;
		case Sip::Comment:
			cout << "Comment";
		}
		cout << endl;
		if (node->key())
		{
			indent(amount);
			cout << "  Key: \"";
			cout.write(node->key(), node->keySize());
			cout << "\"" << endl;
		}
		if (node->value())
		{
			indent(amount);
			cout << "  Value: \"";
			cout.write(node->value(), node->valueSize());
			cout << "\"" << endl;
		}
		if (node->firstChild())
		{
			showYAMLValue(node->firstChild(), amount + 1);
		}
		node = node->nextSibling();
	}
}

int main()
{
	Sip::YAMLDocumentUTF8 doc1;
	doc1.parse(source);
	cout << "Iterating through keys: " << endl;
	showYAMLValue(doc1.firstChild());
	
	Sip::YAMLDocumentUTF16 doc2;
	
	return 0;
}