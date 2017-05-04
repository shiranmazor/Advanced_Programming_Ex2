#include <cctype>
#include <utility>
#include "BattleshipGameAlgoSmart.h"

IBattleshipGameAlgo* GetAlgorithm()
{
	_instancesVec.push_back(new BattleshipGameAlgoSmart());	// Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1];		// Return last instance
}

void BattleshipGameAlgoSmart::setBoard(int player, const char** board, int numRows, int numCols)
{
	this->playerNum = player;
	this->playerName = (player == 0) ? A : B;
	if (this->playerBoard != nullptr)
		delete this->playerBoard; //avoid memory leak

	this->playerBoard = new BattleBoard(board, numRows, numCols);
	this->hostileShipsNum = this->playerBoard->playerToolsNum;
}

bool BattleshipGameAlgoSmart::_canAttack(int i, int j)
{
	return (this->irrelevantCells.find(make_pair(i, j)) == this->irrelevantCells.end() && i < this->playerBoard->R && i >= 0 && j < this->playerBoard->C && j >= 0);
}


bool BattleshipGameAlgoSmart::init(const std::string& path)
{
	for (int i = 0; i < this->playerBoard->R; i++)
	{
		for (int j = 0; j < this->playerBoard->C; j++)
		{
			if (this->playerBoard->board[i][j] != ' ') {
				this->irrelevantCells.insert(make_pair(i, j));
				if (i + 1 < this->playerBoard->R) this->irrelevantCells.insert(make_pair(i + 1, j));
				if (i > 0) this->irrelevantCells.insert(make_pair(i - 1, j));
				if (j + 1 < this->playerBoard->C) this->irrelevantCells.insert(make_pair(i, j + 1));
				if (j > 0) this->irrelevantCells.insert(make_pair(i, j - 1));
			}
		}
	}
	return true;
}


std::pair<int, int> BattleshipGameAlgoSmart::attack()
{
	this->counter += 1;
	if (this->target == nullptr)
	{
		// currently pretty naive, find better logic here
		for (int i = 0; i < this->playerBoard->R; i++)
			for (int j = 0; j < this->playerBoard->C; j++)
				if (this->_canAttack(i, j)) return make_pair(i, j);
	} else // in HUNT mode
	{
		if (this->target->direction == -1)
		{
			if (this->_canAttack(this->target->edges[0].first - 1, this->target->edges[0].second)) return make_pair(this->target->edges[0].first - 1, this->target->edges[0].second);
			if (this->_canAttack(this->target->edges[0].first + 1, this->target->edges[0].second)) return make_pair(this->target->edges[0].first + 1, this->target->edges[0].second);
			if (this->_canAttack(this->target->edges[0].first, this->target->edges[0].second + 1)) return make_pair(this->target->edges[0].first, this->target->edges[0].second + 1);
			if (this->_canAttack(this->target->edges[0].first, this->target->edges[0].second - 1)) return make_pair(this->target->edges[0].first, this->target->edges[0].second - 1);
		}
		if (this->target->direction == 0) // horizontal
		{
			if (this->target->edgeReached != 0) // didn't reach the end of target vessel with edge[0]
			{
				if (this->target->edges[0].second > this->target->edges[1].second && this->_canAttack(this->target->edges[0].first, this->target->edges[0].second + 1))
					return make_pair(this->target->edges[0].first, this->target->edges[0].second + 1);
				if (this->target->edges[0].second < this->target->edges[1].second && this->_canAttack(this->target->edges[0].first, this->target->edges[0].second - 1))
					return make_pair(this->target->edges[0].first, this->target->edges[0].second - 1);
				// in the case that an attack cannot be made from this edge, mark it as reached
				this->target->edgeReached = 0;
			}
			if (this->target->edgeReached != 1) // didn't reach the end of target vessel with edge[1]
			{
				if (this->target->edges[1].second > this->target->edges[0].second && this->_canAttack(this->target->edges[1].first, this->target->edges[1].second + 1))
					return make_pair(this->target->edges[1].first, this->target->edges[1].second + 1);
				if (this->target->edges[1].second < this->target->edges[0].second && this->_canAttack(this->target->edges[1].first, this->target->edges[1].second - 1))
					return make_pair(this->target->edges[1].first, this->target->edges[1].second - 1);
				// in the case that an attack cannot be made from this edge, mark it as reached
				this->target->edgeReached = 1;
			}
		}
		else // this->target->direction == 1, vertical
		{
			if (this->target->edgeReached != 0) // didn't reach the end of target vessel with edge[0]
			{
				if (this->target->edges[0].second > this->target->edges[1].second && this->_canAttack(this->target->edges[0].first + 1, this->target->edges[0].second))
					return make_pair(this->target->edges[0].first + 1, this->target->edges[0].second);
				if (this->target->edges[0].second < this->target->edges[1].second && this->_canAttack(this->target->edges[0].first - 1, this->target->edges[0].second))
					return make_pair(this->target->edges[0].first - 1, this->target->edges[0].second - 1);
				// in the case that an attack cannot be made from this edge, mark it as reached
				this->target->edgeReached = 0;
			}
			if (this->target->edgeReached != 1) // didn't reach the end of target vessel with edge[1]
			{
				if (this->target->edges[1].second > this->target->edges[0].second && this->_canAttack(this->target->edges[1].first + 1, this->target->edges[1].second))
					return make_pair(this->target->edges[1].first + 1, this->target->edges[1].second);
				if (this->target->edges[1].second < this->target->edges[0].second && this->_canAttack(this->target->edges[1].first - 1, this->target->edges[1].second))
					return make_pair(this->target->edges[1].first - 1, this->target->edges[1].second);
				// in the case that an attack cannot be made from this edge, mark it as reached
				this->target->edgeReached = 1;
			}
		}
	}
	// safety, should never get here
	cout << "BattleshipGameAlgoSmart.attack: Something went wrong, can't find next cell to attack (Player " << this->playerNum << ", currently in " << (this->target == nullptr ? "HUNT" : "TARGET") << " mode)" << endl;
	return make_pair(0, 0);
}

void BattleshipGameAlgoSmart::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (player == this->playerName)
	{
		this->irrelevantCells.insert(make_pair(row, col));
		switch (result) {
		case AttackResult::Miss:
			if (this->target != nullptr)
			{
				if (this->target->direction == 0 && (col == this->target->edges[0].second + 1 || col == this->target->edges[0].second - 1)) this->target->edgeReached = 0;
				if (this->target->direction == 0 && (col == this->target->edges[1].second + 1 || col == this->target->edges[1].second - 1)) this->target->edgeReached = 1;
				if (this->target->direction == 1 && (col == this->target->edges[0].first + 1 || col == this->target->edges[0].first - 1)) this->target->edgeReached = 0;
				if (this->target->direction == 1 && (col == this->target->edges[1].first + 1 || col == this->target->edges[1].first - 1)) this->target->edgeReached = 1;
			} 
			break;
		case AttackResult::Hit:
			if (this->target == nullptr)
			{
				this->target = new targetVessel();
				this->target->edges[0] = make_pair(row, col);
				this->target->edges[1] = make_pair(-1, -1);
			} else
			{
				if (this->target->edges[1] == make_pair(-1, -1))
				{
					this->target->edges[1] = make_pair(row, col);
					this->target->direction = this->target->edges[0].second == this->target->edges[1].second;
					
					// add surrounding cells to irrelevantCells according to direction
					if (this->target->direction == 0)
					{
						this->irrelevantCells.insert(make_pair(this->target->edges[0].first + 1, this->target->edges[0].second));
						this->irrelevantCells.insert(make_pair(this->target->edges[0].first - 1, this->target->edges[0].second));
					} else
					{
						this->irrelevantCells.insert(make_pair(this->target->edges[0].first, this->target->edges[0].second + 1));
						this->irrelevantCells.insert(make_pair(this->target->edges[0].first, this->target->edges[0].second - 1));
					}

				} else
				{
					//if (this->target->edges[1].first == row - 1 || this->target->edges[1].first == row + 1 || this->target->edges[1].second == col - 1 || this->target->edges[1].second == col + 1)
					if (abs(this->target->edges[1].first - row) + abs(this->target->edges[1].second - col) == 1) // one cell away from edge[1]
						this->target->edges[1] = make_pair(row, col);
					else
						this->target->edges[0] = make_pair(row, col);
				}

				// add surrounding cells to irrelevantCells according to direction
				if (this->target->direction == 0)
				{
					this->irrelevantCells.insert(make_pair(row + 1, col));
					this->irrelevantCells.insert(make_pair(row - 1, col));
				}
				else
				{
					this->irrelevantCells.insert(make_pair(row, col + 1));
					this->irrelevantCells.insert(make_pair(row, col - 1));
				}
			}
			break;
		case AttackResult::Sink:
			// add surrounding cells to irrelevantCells (last hit, can add all directions)
			this->irrelevantCells.insert(make_pair(row + 1, col));
			this->irrelevantCells.insert(make_pair(row - 1, col));
			this->irrelevantCells.insert(make_pair(row, col + 1));
			this->irrelevantCells.insert(make_pair(row, col - 1));

			delete this->target;
			this->target = nullptr; // back to target mode
			this->hostileShipsNum -= 1;
			break;
		default:
			// safety, should never get here
			cout << "BattleshipGameAlgoSmart.notifyOnAttackResult: Something went wrong, got bad AttackResult" << endl;
			break;
		}
	}
}


