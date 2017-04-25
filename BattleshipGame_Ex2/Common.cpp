#include "Common.h"
#include <algorithm>
#include <ctype.h>

std::vector<std::string> splitString(std::string str, char c)
{
	std::stringstream test(str);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(test, segment, c))
	{
		seglist.push_back(segment);
	}
	return seglist;
}

void replaceAll(string &str, const string &search, const string &replace)
{
	for (size_t pos = 0; ; pos += replace.length())
	{
		// Locate the substring to replace
		pos = str.find(search, pos);
		if (pos == string::npos) break;
		// Replace by erasing and inserting
		str.erase(pos, search.length());
		str.insert(pos, replace);
	}
}

string removeSpaces(string input)
{
	input.erase(remove_if(input.begin(), input.end(), isspace), input.end());
	return input;
}

int getShipSize(char C)
{
	if (tolower(C) == 'b') return 1;
	if (tolower(C) == 'p') return 2;
	if (tolower(C) == 'm') return 3;
	if (tolower(C) == 'd') return 4;
	return 0;//default case
}

int getShipScore(char C)
{
	if (tolower(C) == 'b') return 2;
	if (tolower(C) == 'p') return 3;
	if (tolower(C) == 'm') return 7;
	if (tolower(C) == 'd') return 8;
	return 0;//default case
}

bool isCharValid(char C)
{
	return tolower(C) == 'b' || tolower(C) == 'p' || tolower(C) == 'm' || tolower(C) == 'd';
}