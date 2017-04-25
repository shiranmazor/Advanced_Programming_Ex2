#pragma once
#include <vector>
#include <sstream>


enum Player { A, B };
using namespace std;

std::vector<std::string> splitString(std::string str, char c);
void replaceAll(string &str, const string &search, const string &replace);
string removeSpaces(string input);
int getShipSize(char C);
int getShipScore(char C);
bool isCharValid(char C);
