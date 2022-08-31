#include "ClassFileParser.h"
int main()
{
	CClassFileParser parser("C:\\Users\\Seczone\\Downloads\\gson-2.8.9\\com\\google\\gson\\stream\\JsonReader.class");
	ClassFile* pClassFile = parser.Parse();
	printf("%d\n", pClassFile->methods_count);
}