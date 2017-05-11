#include "BattleBoard.h"

/*
* 	Board validation assisting function, returns:
*	0 - no direction (ship size 1 or adjecent ships error)
*	1 - going right
*	2 - going down
*/
int _getShipDirection(BattleBoard* b, int i, int j)
{
	if (j != b->C - 1)
		if (b->board[i][j + 1] == b->board[i][j])
			return 1;
	if (i != b->R - 1)
		if (b->board[i + 1][j] == b->board[i][j])
			return 2;

	return 0;
}

/*
* Recursively collect all adjacent cells containing the same symbol
*/
void _collect_ship(BattleBoard* b, int i, int j, std::set<pair<int, int>>* s)
{
	char curr = b->board[i][j];
	s->insert(std::make_pair(i, j));

	if (i + 1 < b->R && b->board[i + 1][j] == curr && s->find(std::make_pair(i + 1, j)) == s->end()) _collect_ship(b, i + 1, j, s);
	if (i > 0 && b->board[i - 1][j] == curr && s->find(std::make_pair(i - 1, j)) == s->end()) _collect_ship(b, i - 1, j, s);
	if (j + 1 < b->C && b->board[i][j + 1] == curr && s->find(std::make_pair(i, j + 1)) == s->end()) _collect_ship(b, i, j + 1, s);
	if (j > 0 && b->board[i][j - 1] == curr && s->find(std::make_pair(i, j - 1)) == s->end()) _collect_ship(b, i, j - 1, s);

	return;
}

/*
*  Check if the board is valid and print relevant messages if it isn't,
*  while doing so, also initialize 'ships' Map containing board cells
*  leading to Vessel object representing the different Vessels on the board.
*/
bool BattleBoard::isBoardValid(vector<string>& error_messages)
{
	int countA = 0;
	int countB = 0;
	int totalShape = 0;
	set<pair<int, int>> checkedCells, temp;
	bool allI, allJ;
	bool badShape[8] = { false };
	bool tooClose = false;
	Vessel* currShip;

	for (int i = 0; i < this->R; i++)
	{
		for (int j = 0; j < this->C; j++)
		{
			// ignore empty or already checked boxes (in case of full ship scan)
			if (this->board[i][j] == ' ' || checkedCells.find(std::make_pair(i, j)) != checkedCells.end()) continue;

			// collect all adjacent cells contaning the same symbol
			_collect_ship(this, i, j, &temp);
			allI = true;
			allJ = true;
			for (auto const& element : temp)
			{
				allI = element.first == i && allI;
				allJ = element.second == j && allJ;
			}

			// determine ship shape and size
			if ((!allJ && !allI) || temp.size() != getShipSize(this->board[i][j]))
			{
				badShape[ship2idx.at(this->board[i][j])] = true;
				for (auto const& element : temp)
					checkedCells.insert(element);
				//this->board[element.first][element.second] = ' ';
			}
			else // ship shape and size are correct
			{
				if (isPlayerChar(A, this->board[i][j])) countA++;
				else countB++;

				// add ship to the game board 'ships' Map and check for adjacent ships
				currShip = new Vessel(this->board[i][j]);
				for (auto const& element : temp)
				{
					checkedCells.insert(element);
					this->ships[makeKey(element)] = currShip;
					if ((element.first > 0 && this->board[element.first - 1][element.second] != ' ' && this->board[element.first - 1][element.second] != this->board[i][j]) ||
						(element.second > 0 && this->board[element.first][element.second - 1] != ' ' && this->board[element.first][element.second - 1] != this->board[i][j]) ||
						(element.first < this->R - 1 && this->board[element.first + 1][element.second] != ' ' && this->board[element.first + 1][element.second] != this->board[i][j]) ||
						(element.second < this->C - 1 && this->board[element.first][element.second + 1] != ' ' && this->board[element.first][element.second + 1] != this->board[i][j]))
					{
						tooClose = true;
					}
				}
			}
			temp.clear();
		}
	}

	// print out relevant errors
	for (int i = 0; i < 8; i++)
	{
		if (badShape[i])
		{
			string error = "Wrong size or shape for ship ";
			error.append(&(idx2ship[i]));
			error.append(" for player ");
			error.append(((i < 4) ? "A" : "B"));
			error_messages.push_back(error);
		}
			
		totalShape += badShape[i];
	}

	if (countA > this->playerToolsNum) error_messages.push_back( "Too many ships for player A" );
	if (countA < this->playerToolsNum) error_messages.push_back("Too few ships for player A");
	if (countB > this->playerToolsNum) error_messages.push_back("Too many ships for player B");
	if (countB < this->playerToolsNum) error_messages.push_back("Too few ships for player B");

	if (tooClose) error_messages.push_back("Adjacent Ships on Board");

	return (countA == this->playerToolsNum && countB == this->playerToolsNum && !tooClose && totalShape == 0);
}

pair<int, int> BattleBoard::CalcScore()
{
	pair<int, int> scores = std::make_pair(0, 0);
	set<Vessel*> seenVessels;

	for (auto const& element : this->ships)
	{
		if (element.second->size <= element.second->hitNum && seenVessels.find(element.second) == seenVessels.end())
		{
			//ship has sinked!
			seenVessels.insert(element.second);
			if (element.second->player == B)
				scores.first += getShipScore(element.second->type);
			else
				scores.second += getShipScore(element.second->type);
		}
	}
	return scores;
}

int BattleBoard::CheckVictory()
{
	int winner = -1;
	int countA = 0, countB = 0;
	set<Vessel*> seenVessels;

	for (auto const& element : this->ships)
	{
		if (element.second->size <= element.second->hitNum && seenVessels.find(element.second) == seenVessels.end())
		{
			seenVessels.insert(element.second);
			if (element.second->player == A) countA++;
			else countB++;
		}
	}

	if (countA == this->playerToolsNum) winner = B;
	else if (countB == this->playerToolsNum) winner = A;

	return winner;
}

void BattleBoard::getPlayerBoard(Player player, char** &pBoard) const
{
	pBoard = new char*[this->R];
	for (int i = 0; i < this->C; i++)
		pBoard[i] = new char[this->C];

	for (int i = 0; i < this->R; i++)
	{
		for (int j = 0; j < this->C; j++)
		{
			if (isPlayerChar(player, this->board[i][j]))
				pBoard[i][j] = this->board[i][j];
			else
				pBoard[i][j] = ' ';
		}
	}
}

AttackResult BattleBoard::performGameMove(int p, pair<int, int> move)
{
	pair<int, int> boardMove = make_pair(move.first - 1, move.second - 1);
	char c = this->board[boardMove.first][boardMove.second];
	if (!isspace(c)) {
		if (isAlreadyHit(c)) {
			return (this->ships[makeKey(boardMove)]->hitNum == this->ships[makeKey(boardMove)]->size) ? AttackResult::Miss : AttackResult::Hit;
		}
		if (isupper(c) || islower(c)) {
			this->board[boardMove.first][boardMove.second] = isupper(c) ? HitMarkA : HitMarkB;
			this->ships[makeKey(boardMove)]->hitNum++;
			return (this->ships[makeKey(boardMove)]->hitNum == this->ships[makeKey(boardMove)]->size) ? AttackResult::Sink : AttackResult::Hit;
		}
	}
	return AttackResult::Miss;
}
