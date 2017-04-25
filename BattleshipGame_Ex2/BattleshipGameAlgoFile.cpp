#include "BattleshipGameAlgoFile.h"
#include <cctype>
#include <utility>

#define isPlayerTool(x, y) (x==A && (isupper(y) && !isspace(y)) || (x==B && (islower(y) && !isspace(y))))

void BattleshipGameAlgoFile::setBoard(int player, const char** board, int numRows, int numCols)
{
	if (this->playerBoard != nullptr)
		delete this->playerBoard; //avoid memory leak

	this->playerBoard = new BattleBoard(board, numRows, numCols);

}
bool BattleshipGameAlgoFile::init(const std::string& path)
{
	return true;
}


bool BattleshipGameAlgoFile::isAttackLineValid(string line)
{

	string newLine = removeSpaces(line);
	//check there is excatly one comma!
	vector<string> splitLine = splitString(newLine, ',');
	if (splitLine.size() != 2)
		return false;
	for (vector<string>::const_iterator i = splitLine.begin(); i != splitLine.end(); ++i)
	{
		//check number in the range
		string value = *i;
		int num = stoi(value);
		if (!(num >= 1 && num <= 10))
			return false;
	}
	return true;
}


std::pair<int, int> BattleshipGameAlgoFile::attack()
{
	//read first line from attack file
	pair<int, int> attackMove;
	string line;
	if (movesQueue.size() == 0)
		return make_pair(-1, -1);
	
	line = movesQueue.front();
	movesQueue.pop();
	while (!isAttackLineValid(line))
	{
		line = movesQueue.front();
		movesQueue.pop();
		if (movesQueue.size() == 0)
			return make_pair(-1, -1);	
	}

	//if reach here attack line is valid- parse it
	line = removeSpaces(line);
	vector<string> splitLine = splitString(line, ',');
	//splitLine has 2 items
	attackMove.first = stoi(splitLine[0]) - 1;
	attackMove.second = stoi(splitLine[1]) - 1;

	return attackMove;
}

void BattleshipGameAlgoFile::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	char c = this->playerBoard->board[row][col];
	bool isOppVessel = (islower(c) && this->playerName == A) || (isupper(c) && this->playerName == B);
	switch (result) {
	case AttackResult::Miss:
		this->playerBoard->board[row][col] = isOppVessel ? OpMissMark : MyMissMark;
		break;
	case AttackResult::Hit:
		this->playerBoard->board[row][col] = isOppVessel ? OpHitMark : MyHitMark;
		break;
	case AttackResult::Sink:
		this->playerBoard->board[row][col] = isOppVessel ? OpSinkMark : MySinkMark; //TODO: mark all the other hits as sink as well
		break;
	default:
		//TODO: print err (unknown attackres)
		break;
	}
}
