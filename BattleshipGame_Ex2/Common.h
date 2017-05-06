#pragma once
#include <vector>
#include <sstream>

#define MyHitMark '*'
#define OpHitMark '#'
#define MyMissMark '~'
#define OpMissMark '^'
#define MySinkMark '!'
#define OpSinkMark '?'

#define _make_pair(x, y) make_pair(x + 1, y + 1)

enum Player { A, B };
using namespace std;

std::vector<std::string> splitString(std::string str, char c);
void replaceAll(string &str, const string &search, const string &replace);
string removeSpaces(string input);
int getShipSize(char C);
int getShipScore(char C);
bool isCharValid(char C);
