#include "Game.h"
#include <tuple>
#include "BattleBoard.h"

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
	string algoName1;
	string algoName2;
	string dll2_path = gameFiles[2];
	// Load dynamic library 1
	HINSTANCE hDll1 = LoadLibraryA(dll1_path.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (!hDll1)
	{
		std::cout << "could not load the dynamic library of first algo all" << std::endl;
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
		std::cout << "could not load the dynamic library of first algo all" << std::endl;
		return false;
	}

	// Get function pointer of dll 2
	getAlgorithmFunc1 = (GetAlgorithmFuncType)GetProcAddress(hDll2, "GetAlgorithm");
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

bool CheckValidPath(vector<string> gameFiles, string path)
{
	bool sboard = false, one_dll = false, two_dll = false;
	if (gameFiles.size() == 3)
		return true;

	//check which file is missing and print message
	vector<string>::iterator fileIt;
	for (fileIt = gameFiles.begin(); fileIt != gameFiles.end(); ++fileIt)
	{
		if (fileIt->find("sboard") != std::string::npos)
			sboard = true;
		if (fileIt->find("dll") != std::string::npos)
			one_dll = true;
	}

	if (!sboard)
	{
		cout << "Missing board file (*.sboard) looking in path:" + path << endl;
		return false;
	}
	
	return true;
}


int PlayGame(vector<string> gameFiles)
{

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

	//create players object
	BattleshipGameAlgoFile* playerA = new BattleshipGameAlgoFile(A, gameFiles[1]);
	BattleshipGameAlgoFile* playerB = new BattleshipGameAlgoFile(B, gameFiles[2]);
	mainBoard->getPlayerBoard(playerA->playerName, playerBoardA);
	mainBoard->getPlayerBoard(playerB->playerName, playerBoardB);
	//playerA->setBoard(const_cast<const char**>(playerBoardA), mainBoard->R, mainBoard->C);
	//playerB->setBoard(const_cast<const char**>(playerBoardB), mainBoard->R, mainBoard->C);
	pair<int, int> attackMove;
	//we starts with player A
	BattleshipGameAlgoFile* currentPlayer = playerA;
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
			cout << "Player A won" << endl;
		else if (winPlayer == B)
			cout << "Player B won" << endl;
	}
	//points
	cout << "Points:" << endl;
	pair<int, int> gameScore = mainBoard->CalcScore();
	cout << "Player A: " << gameScore.first << endl;
	cout << "Player B: " << gameScore.second << endl;

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
	vector<string> gameFiles;
	getGameFiles(path, gameFiles);
	if (!CheckValidPath(gameFiles, path))
	{
		std::cout << "Error game files are missing, Exiting game" << endl;
		return -1;
	}
	return 	PlayGame(gameFiles);


}