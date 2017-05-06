#include "Game.h"



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

bool loadAlgoDllAndInitGame(string folder, vector<string> gameFiles , BattleBoard* mainBoard,
	tuple<IBattleshipGameAlgo*, IBattleshipGameAlgo*>& players)
{
	
	GetAlgorithmFuncType getAlgorithmFunc1, getAlgorithmFunc2;
	char** playerBoardA = NULL; char** playerBoardB = NULL;
	string dll1_path = gameFiles[1];
	string dll2_path = gameFiles[2];
	// Load dynamic library 1
	HINSTANCE hDll1 = LoadLibraryA(dll1_path.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (!hDll1)
	{
		std::cout << "Cannot load dll" + dll1_path << std::endl;
		return false;
	}
	// Get function pointer of dll 1
	getAlgorithmFunc1 = (GetAlgorithmFuncType)GetProcAddress(hDll1, "GetAlgorithm");
	if (!getAlgorithmFunc1)
	{
		std::cout << "Cannot load dll" + dll1_path << std::endl;
		return false;
	}
	//now let's perform setboard and init as the instruction ordered:
	IBattleshipGameAlgo* playerA = getAlgorithmFunc1();
	mainBoard->getPlayerBoard(A, playerBoardA);
	playerA->setBoard(0, const_cast<const char**>(playerBoardA), mainBoard->R, mainBoard->C);
	//call init
	if (!playerA->init(folder))
	{
		std::cout << "Algorithm initialization failed for dll:" + dll1_path << endl;
		if (playerBoardA != NULL)
			delete[] playerBoardA;
		return false;
	}
	
	// Load dynamic library 2
	HINSTANCE hDll2 = LoadLibraryA(dll2_path.c_str()); 
	if (!hDll2)
	{
		std::cout << "Cannot load dll" + dll2_path << std::endl;
		return false;
	}

	// Get function pointer of dll 2
	getAlgorithmFunc2 = (GetAlgorithmFuncType)GetProcAddress(hDll2, "GetAlgorithm");
	if (!getAlgorithmFunc2)
	{
		std::cout << "Cannot load dll" + dll1_path << std::endl;
		return false;
	}
	//now let's perform setboard and init for playerB as the instruction ordered:
	IBattleshipGameAlgo* playerB = getAlgorithmFunc2();
	mainBoard->getPlayerBoard(B, playerBoardB);
	playerB->setBoard(1, const_cast<const char**>(playerBoardB), mainBoard->R, mainBoard->C);
	//call init
	if (!playerB->init(folder))
	{
		std::cout << "Algorithm initialization failed for dll:" + dll2_path << endl;
		if (playerBoardB != NULL)
			delete[] playerBoardB;
		if (playerBoardA != NULL)
			delete[] playerBoardA;
		return false;
	}
	players = make_tuple(playerA, playerB);
	//outside loop, avoid memory leak
	if (playerBoardA != NULL)
		delete[] playerBoardA;
	if (playerBoardB != NULL)
		delete[] playerBoardB;
	return true;
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
bool CheckExistingDlls(vector<string> gameFiles, string path, vector<string>& error_messages)
{
	int dllCounter = 0;

	for (int i = 0; i < gameFiles.size(); i++)
	{
		if (gameFiles[i].find("dll") != std::string::npos)
			dllCounter++;
	}
	if (dllCounter < 2)//need at least 2 dlls!!
	{
		error_messages.push_back("Missing an algorithm (dll) file looking in path:" + path);
		return false;
	}
	return true;
}

void gotoxy(int line, int column)
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsole, coord);
}

void setTextColor(int color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}


int PlayGame(string path, vector<string> gameFiles, tuple<IBattleshipGameAlgo*, IBattleshipGameAlgo*>& players, 
	bool isQuiet, int delay, BattleBoard* mainBoard)
{
	Player onePlayerName; bool victory = false; int winPlayer = 2;
	//create players object
	IBattleshipGameAlgo* playerA = get<0>(players);
	IBattleshipGameAlgo* playerB = get<1>(players);

	// init board on console
	if (!isQuiet)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		ShowConsoleCursor(false);
		for (int i = 0; i < mainBoard->R; i++)
		{
			for (int j = 0; j < mainBoard->C; j++)
			{
				char c = mainBoard->board[i][j];
				int color = WHITE;
				if (isPlayer(c)) color = isupper(c) ? GREEN : RED;
				SetConsoleTextAttribute(hConsole, color);
				(j != mainBoard->C - 1) ? cout << color << c : cout << color << c << endl;
			}
		}
		ShowConsoleCursor(true);
		Sleep(delay);
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
			if (onePlayerGame && onePlayerName == currentPlayer->playerName)
			{
				//exit while loop
				break;
			}
			else if (!onePlayerGame)
			{
				onePlayerGame = true;
				currentPlayer = swapPlayer(currentPlayer, playerA, playerB);
				onePlayerName = currentPlayer->playerName;
			}
			else
				currentPlayer = swapPlayer(currentPlayer, playerA, playerB);

			continue;
		}
		AttackResult moveRes = mainBoard->performGameMove(currentPlayer->playerName, attackMove);

		// update board on consul 
		if ((!isQuiet) && (moveRes != AttackResult::Miss))
		{
			char c = mainBoard->board[attackMove.first][attackMove.second];
			int color = currentPlayer->playerName == A ? GREEN : RED;

			ShowConsoleCursor(false);
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			gotoxy(attackMove.first, attackMove.second);
			SetConsoleTextAttribute(hConsole, GREEN);
			cout << color << c;
			ShowConsoleCursor(true);
			Sleep(delay);
		}

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

	return 0;
}

int main(int argc, char **argv)
{
	string path; bool boardValid = true; bool dllExist = true;
	vector<string> error_messages;
	BattleBoard* mainBoard;
	vector<string> gameFiles;
	vector<tuple<string, HINSTANCE, GetAlgorithmFuncType>>  dll_vec;
	char the_path[256];

	//setup defult parameters
	_getcwd(the_path, 255);
	path = std::string(the_path);
	bool isQuiet = false;
	int delay = 100;
	// parse command line parameters
	int i = 0;
	while (i < argc)
	{
		if (strcmp(argv[i], "-quite") == 0) {
			isQuiet = true;
		}
		else if (strcmp(argv[i], "-delay") == 0) {
			delay = atoi(argv[i + 1]);
			i++;
		}
		else
		{
			path = path = argv[i];
		}
		i++;
	}
	if (!dirExists(path))
	{
		std::cout << "Wrong path:" + path << endl;
		return -1;
	}
	//path is valid, continue
	getGameFiles(path, gameFiles);
	if (gameFiles.size() == 0 || (gameFiles.size() > 0 && gameFiles[0].find("sboard") == std::string::npos))
		error_messages.push_back("Missing board file (*.sboard) looking in path:" + path);
		
	else//sboard file exist
	{
		mainBoard = new BattleBoard(gameFiles[0]);
		if (!mainBoard->isBoardValid(error_messages))//check board is valid and initialize board ships, output error messages like in ex1
			boardValid = false;
	}
	dllExist = CheckExistingDlls(gameFiles, path, error_messages);
	if (!dllExist || !boardValid || error_messages.size() > 0)//errors number 2
	{
		//output error messages
		for (int i = 0; i < error_messages.size(); i++)
			cout << error_messages[i] << endl;
		return -1;
	}

	//load dll algo
	tuple<IBattleshipGameAlgo*, IBattleshipGameAlgo*> players;
	if (!loadAlgoDllAndInitGame(path,gameFiles,mainBoard,players))
		return -1;
	int ret = PlayGame(path, gameFiles, players, isQuiet, delay, mainBoard);
	closeDLLs(dll_vec);
	return ret;

}