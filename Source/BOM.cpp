#include <iostream>
using std::cout;
using std::endl;
#include "SipYAML.hpp"
using Sip::checkBOM;

std::ostream& operator<< (std::ostream& stream, Sip::BOM const& bom)
{
    stream << "(BOM: ";
	switch(bom)
	{
	case Sip::UTF8:
		stream << "UTF-8";
		break;
	case Sip::UTF16LE:
		stream << "UTF-16LE";
		break;
	case Sip::UTF16BE:
		stream << "UTF-16BE";
		break;
	case Sip::UTF32LE:
		stream << "UTF-32LE";
		break;
	case Sip::UTF32BE:
		stream << "UTF-32BE";
		break;
	}
	stream << ")";
	return stream;
}

int main()
{
	cout << checkBOM("\x00\x00\xFE\xFF") << endl;	// UTF-32BE
	cout << checkBOM("\x00\x00\x00\x00") << endl;	// UTF-32BE
	cout << checkBOM("\xFF\xFE\x00\x00") << endl;	// UTF-32LE
	cout << checkBOM("\x01\x00\x00\x00") << endl;	// UTF-32LE
	cout << checkBOM("\xFE\xFF-") << endl;			// UTF-16BE
	cout << checkBOM("\x00\x0x-") << endl;			// UTF-16BE
	cout << checkBOM("\xFF\xFE-") << endl;			// UTF-16LE
	cout << checkBOM("\x01\x00-") << endl;			// UTF-16LE
	cout << checkBOM("\xEF\xBB\xBF") << endl;		// UTF-8
	cout << checkBOM("---") << endl;				// UTF-8 Default
	return 0;
}