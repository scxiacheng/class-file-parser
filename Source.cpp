#include "ClassFileParser.h"
int main()
{
	CClassFileParser parser("ClassEntryLookUp$.class");
	ClassFile* pClassFile = parser.Parse();
	parser.Release();
	delete pClassFile;
	return 0;
}