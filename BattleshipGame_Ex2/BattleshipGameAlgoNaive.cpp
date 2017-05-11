#include "BattleshipGameAlgoNaive.h"
#include <cctype>
#include <utility>

IBattleshipGameAlgo* GetAlgorithm()
{
	_instancesVec.push_back(new BattleshipGameAlgoNaive());	// Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1];		// Return last instance
}

bool BattleshipGameAlgoNaive::_canAttack(int i, int j) const
{
	return ((i + 1 == this->playerBoard->R || this->playerBoard->board[i + 1][j] == ' ') &&
			(i == 0 || this->playerBoard->board[i - 1][j] == ' ') &&
			(j + 1 == this->playerBoard->C || this->playerBoard->board[i][j + 1] == ' ') &&
			(j == 0 || this->playerBoard->board[i][j - 1] == ' '));
}

void BattleshipGameAlgoNaive::setBoard(int player, const char** board, int numRows, int numCols)
{
	this->playerNum = player;
	delete this->playerBoard; //avoid memory leak
	this->playerBoard = new BattleBoard(board, numRows, numCols);
}

bool BattleshipGameAlgoNaive::init(const std::string& path)
{	
	for (int i = 0; i < this->playerBoard->R; i++)
	{
		for (int j = 0; j < this->playerBoard->C; j++)
		{
			if (this->playerBoard->board[i][j] != ' ') continue;
			if (_canAttack(i, j)) attackQueue.push(_make_pair(i, j));
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
	// nothing to do here..
}


