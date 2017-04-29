#include "BattleshipGameAlgoNaive.h"
#include <cctype>
#include <utility>


std::pair<int, int> BattleshipGameAlgoNaive::attack()
{
	pair<int, int> attack = this->attackQueue.front();
	this->attackQueue.pop();
	return attack;
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
}


