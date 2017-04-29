#include "BattleshipGameAlgoNaive.h"
#include <cctype>
#include <utility>

IBattleshipGameAlgo* GetAlgorithm()
{
	_instancesVec.push_back(new BattleshipGameAlgoNaive());	// Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1];		// Return last instance
}

void BattleshipGameAlgoNaive::setBoard(int player, const char** board, int numRows, int numCols)
{
	this->playerNum = player;
	this->playerName = (player == 0) ? A : B;
	if (this->playerBoard != nullptr)
		delete this->playerBoard; //avoid memory leak

	this->playerBoard = new BattleBoard(board, numRows, numCols);

}

bool _canAttack(BattleBoard* b, int i, int j)
{	
	int r = (j < (b->C - 1)) ? 1 : 0;
	int l = (j > 0) ? 1 : 0;
	int d = (i < (b->R - 1)) ? 1 : 0;
	int u = (i > 0) ? 1 : 0;

	for (int n = i - u; n < i + d; n++)
	{
		for (int m = j - l; m < j + r; m++)
		{
			if (b->board[n][m] != ' ') return false;
		}
	}
	return true;
}


bool BattleshipGameAlgoNaive::init(const std::string& path)
{	
	for (int i = 0; i < this->playerBoard->R; i++)
	{
		for (int j = 0; j < this->playerBoard->C; j++)
		{
			if (this->playerBoard->board[i][j] != ' ') continue;
			if (_canAttack(this->playerBoard, i, j))
			{
				attackQueue.push(std::make_pair(i, j));
			}
		}
	}
	return true;
}


std::pair<int, int> BattleshipGameAlgoNaive::attack()
{
	while (!this->attackQueue.empty())
	{
		pair<int, int> attack = this->attackQueue.front();
		this->attackQueue.pop();
		return attack;
	}
	return make_pair(-1, -1);
}

void BattleshipGameAlgoNaive::notifyOnAttackResult(int player, int row, int col, AttackResult result)
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


