#include "Game.h"
#include <tuple>
#include "BattleBoard.h"
#include "BattleshipGameAlgoFile.h"

void gotoxy(short col, short row)
{
	static HANDLE h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { col, row };
	SetConsoleCursorPosition(h, c);
}
void setTextColor(int color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}


/*
* get current working directory path
*/
IBattleshipGameAlgo* swapPlayer(IBattleshipGameAlgo* current, IBattleshipGameAlgo* pA, IBattleshipGameAlgo* pB)
{
	if (current->playerName == pA->playerName)
		return pB;
	else
		return pA;
}

bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

void getGameFiles(string folder, vector<string> & gameFiles)
{
	WIN32_FIND_DATAA search_data;
	HANDLE handle;
	memset(&search_data, 0, sizeof(WIN32_FIND_DATAA));

	//start with sboard files
	string sboardPath = folder + "\\*.sboard";
	handle = FindFirstFileA(sboardPath.c_str(), &search_data);

	if (handle != INVALID_HANDLE_VALUE)
	{
		//check file extension
		string filename(search_data.cFileName);

		if (filename.find("sboard") != std::string::npos)
		{
			//found sboard file
			string fullPath = folder + "\\" + filename;
			gameFiles.push_back(fullPath);
		}
	}

	//search 2 dll files:
	string dll1 = folder + "\\*.dll";
	handle = FindFirstFileA(dll1.c_str(), &search_data);

	if (handle != INVALID_HANDLE_VALUE)
	{
		//check file extension
		string filename(search_data.cFileName);
		if (filename.find("dll") != std::string::npos)
		{
			//found  file
			string fullPath = folder + "\\" + filename;
			gameFiles.push_back(fullPath);
		}
	}
	//find dll2:
	if (FindNextFileA(handle, &search_data))
	{
		//check file extension
		string filename(search_data.cFileName);
		if (filename.find("dll") != std::string::npos)
		{
			//found  file
			string fullPath = folder + "\\" + filename;
			gameFiles.push_back(fullPath);
		}
	}
}

bool loadAlgoDllFiles(string folder, vector<string> gameFiles
	,vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>> & dll_vec)
{
	GetAlgorithmFuncType getAlgorithmFunc1;
	GetAlgorithmFuncType getAlgorithmFunc2;
	string dll1_path = gameFiles[1];
	string algoName1 = dll1_path;
	string dll2_path = gameFiles[2];
	string algoName2 = dll2_path;
	// Load dynamic library 1
	HINSTANCE hDll1 = LoadLibraryA(dll1_path.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (!hDll1)
	{
		std::cout << "Cannot load dll"+ dll1_path << std::endl;
		return false;
	}

	// Get function pointer of dll 1
	getAlgorithmFunc1 = (GetAlgorithmFuncType)GetProcAddress(hDll1, "GetAlgorithm");
	if (!getAlgorithmFunc1)
	{
		std::cout << "could not load function GetShape()" << std::endl;
		return false;
	}
	dll_vec.push_back(make_tuple(algoName1, hDll1, getAlgorithmFunc1));

	// Load dynamic library 1
	HINSTANCE hDll2 = LoadLibraryA(dll2_path.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (!hDll2)
	{
		std::cout << "Cannot load dll" + dll2_path << std::endl;
		return false;
	}

	// Get function pointer of dll 2
	getAlgorithmFunc2 = (GetAlgorithmFuncType)GetProcAddress(hDll2, "GetAlgorithm");
	if (!getAlgorithmFunc2)
	{
		std::cout << "could not load function GetShape()" << std::endl;
		return false;
	}
	dll_vec.push_back(make_tuple(algoName2, hDll2, getAlgorithmFunc2));

	if (dll_vec.size() == 2)
		return true;
	else
		return false;
}

void closeDLLs(vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>> & dll_vec)
{
	vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>>::iterator vitr;
	// close all the dynamic libs we opened
	for (vitr = dll_vec.begin(); vitr != dll_vec.end(); ++vitr)
	{
		FreeLibrary(get<1>(*vitr));
	}
}
bool CheckValidPath(vector<string> gameFiles, string path)
{
	bool sboard = false;
	if (gameFiles.size() == 3)
		return true;

	//check which file is missing and print message
	vector<string>::iterator fileIt;
	for (fileIt = gameFiles.begin(); fileIt != gameFiles.end(); ++fileIt)
	{
		if (fileIt->find("sboard") != std::string::npos)
			sboard = true;
	}

	if (!sboard)
	{
		std::cout << "Missing board file (*.sboard) looking in path:" + path << endl;
		return false;
	}

	//missing dll file
	std::cout << "Missing an algorithm (dll) file looking in path:" + path << endl;
	return false;



}


int PlayGame(string path, vector<string> gameFiles, 
	vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>> dll_vec)
{
	//create Ibattleship vector
	vector<IBattleshipGameAlgo*> algo_vec;
	bool victory = false;
	int winPlayer = 2;
	char** playerBoardA = NULL;
	char** playerBoardB = NULL;
	BattleBoard* mainBoard = new BattleBoard(gameFiles[0]);
	//check board is valid and initialize board ships
	if (!mainBoard->isBoardValid())
	{
		//  invalid board
		return -1;
	}
	//add algorithms to algo_vec vector
	algo_vec.push_back(get<2>(dll_vec[0])());
	algo_vec.push_back(get<2>(dll_vec[1])());

	//create players object
	IBattleshipGameAlgo* playerA = (algo_vec[0]);
	IBattleshipGameAlgo* playerB = (algo_vec[1]);
	mainBoard->getPlayerBoard(A, playerBoardA);
	mainBoard->getPlayerBoard(B, playerBoardB);
	playerA->setBoard(0, const_cast<const char**>(playerBoardA), mainBoard->R, mainBoard->C);
	playerA->setBoard(1, const_cast<const char**>(playerBoardB), mainBoard->R, mainBoard->C);
	string fullFileName1 = get<0>(dll_vec[0]);
	string fullFileName2 = get<0>(dll_vec[1]);
	//call init
	if (!playerA->init(path))
	{
		std::cout << "Algorithm initialization failed for dll:"+ fullFileName1 << endl;
		return -1;
	}
		
	if (!playerB->init(path))
	{
		std::cout << "Algorithm initialization failed for dll:" + fullFileName2 << endl;
		return -1;
	}
	pair<int, int> attackMove;
	//we starts with player A
	IBattleshipGameAlgo* currentPlayer = playerA;
	bool onePlayerGame = false;

	while (!victory)
	{
		//set current player board
		attackMove = currentPlayer->attack();
		if (attackMove.first == -1 && attackMove.second == -1)
		{
			if (onePlayerGame)
			{
				//exit while loop
				break;
			}
			onePlayerGame = true;
			currentPlayer = swapPlayer(currentPlayer, playerA, playerB);
			continue;
		}
		AttackResult moveRes = mainBoard->performGameMove(currentPlayer->playerName, attackMove);

		//notify both players on the moveAttak results
		playerA->notifyOnAttackResult(currentPlayer->playerName, attackMove.first, attackMove.second, moveRes);
		playerB->notifyOnAttackResult(currentPlayer->playerName, attackMove.first, attackMove.second, moveRes);

		//check victory:
		winPlayer = mainBoard->CheckVictory();
		if (winPlayer == A || winPlayer == B)
		{
			victory = true;
			break;
		}

		// if Miss or self hit next turn is of the other player.
		if (moveRes == AttackResult::Miss || (moveRes != AttackResult::Miss &&
			isSelfHit(currentPlayer->playerName, mainBoard->board[attackMove.first][attackMove.second]))
			currentPlayer = swapPlayer(currentPlayer, playerA, playerB);

	}

	if (victory)
	{
		if (winPlayer == A)
			std::cout << "Player A won" << endl;
		else if (winPlayer == B)
			std::cout << "Player B won" << endl;
	}
	//points
	std::cout << "Points:" << endl;
	pair<int, int> gameScore = mainBoard->CalcScore();
	std::cout << "Player A: " << gameScore.first << endl;
	std::cout << "Player B: " << gameScore.second << endl;

	//outside loop, avoid memory leak
	if (playerBoardA != NULL)
		delete[] playerBoardA;
	if (playerBoardB != NULL)
		delete[] playerBoardB;

	//delete objects
	delete playerA;
	delete playerB;
	delete mainBoard;
	return 0;
}



int main(int argc, char **argv)
{
	string path;
	vector<string> gameFiles;
	vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>>  dll_vec;
	if (argc < 2)
	{
		char the_path[256];
		//use working directory
		_getcwd(the_path, 255);
		path = std::string(the_path);
	}
	else
		path = argv[1];
	if (!dirExists(path))
	{
		std::cout << "Wrong path:" + path << endl;
		return -1;
	}

	//path is valid, continue

	getGameFiles(path, gameFiles);
	if (!CheckValidPath(gameFiles, path))
	{
		std::cout << "Error game files are missing, Exiting game" << endl;
		return -1;
	}

	//load dll algo
	if (!loadAlgoDllFiles(path, gameFiles, dll_vec))
		return -1;

	int ret = PlayGame(path, gameFiles, dll_vec);
	closeDLLs(dll_vec);
	return ret;



}