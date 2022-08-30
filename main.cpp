#include "ClassFileParser.h"
using iptr = int*;
int main()
{
	CClassFileParser parser("ObjectMapper$2.class");
	parser.Parse();
	parser.Release();
	return 0;
}
