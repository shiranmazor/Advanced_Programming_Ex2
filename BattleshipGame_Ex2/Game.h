#pragma once
#include "BattleshipGameAlgoFile.h"
#include <iostream>
#include <string>
#include <direct.h>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include "time.h"



using namespace std;

#define GREEN 10;
#define RED 12;
#define BLUE 9;
#define PURPLE  13;
#define YELLOW 14;
#define WHITE 15;

bool CheckValidPath(vector<string> gameFiles, string path);
void getGameFiles(string folder, vector<string> & gameFiles);
bool dirExists(const std::string& dirName_in);
void gotoxy(short col, short row); //added for future using
void setTextColor(int color);//added for future using
							 /*
							 * performing game steps and finish the game in case of victory or finished attack steps
							 * gameFiles contains - sboard file, attack-a file,attack-b file by this order!
							 */
int PlayGame(vector<string> gameFiles);


