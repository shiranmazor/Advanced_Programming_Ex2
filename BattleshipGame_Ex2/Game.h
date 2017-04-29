#pragma once
#include "IBattleshipGameAlgo.h"
#include <iostream>
#include <string>
#include <direct.h>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include "time.h"
#include "Common.h"



using namespace std;

#define GREEN 10;
#define RED 12;
#define BLUE 9;
#define PURPLE  13;
#define YELLOW 14;
#define WHITE 15;

// define function of the type we expect
typedef IBattleshipGameAlgo *(*GetAlgorithmFuncType)();


/*
 * check if the files below exist in the given path:
 * one sboard file and 2 dll files
 * if not return false
 */
bool CheckValidPath(vector<string> gameFiles, string path);
/*
 * read needed game files from the directory and feel the gameFiles vector with the files path.
 * sboard file + 2 dll files
 */
void getGameFiles(string folder, vector<string> & gameFiles);
bool loadAlgoDllFiles(string folder, vector<string> gameFiles
	, vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>> & dll_vec);
void closeDLLs(vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>> & dll_vec);
bool dirExists(const std::string& dirName_in);
void gotoxy(short col, short row); //added for future using
void setTextColor(int color);//added for future using
							 /*
							 * performing game steps and finish the game in case of victory or finished attack steps
							 * gameFiles contains - sboard file, attack-a file,attack-b file by this order!
							 */
int PlayGame(vector<string> gameFiles, vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>> dll_vec);


