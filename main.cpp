#include "ClassFileParser.h"
using iptr = int*;
int main()
{
	CClassFileParser parser("D:\\Code\\External\\ClassAnalyzer\\cache\\com\\fasterxml\\jackson\\databind\\ObjectMapper$2.class");
	parser.Parser();
	parser.Release();
	return 0;
}