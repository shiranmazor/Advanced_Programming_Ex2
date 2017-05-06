#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include "Common.h"

#define irrelevnatCell '-'

using namespace std;

const char shipsBySize[4] = { 'd', 'm', 'p', 'b' };

class targetVessel
{
public:
	int direction = -1;
	int edgeReached = -1;
	pair<int, int> edges[2];
};

class BattleshipGameAlgoSmart : public IBattleshipGameAlgo
{
public:
	BattleBoard* playerBoard = nullptr;
	targetVessel* target = nullptr;
	int hostileShipsNum = -1;

	// Blocking Copy and Assignment
	BattleshipGameAlgoSmart(const BattleshipGameAlgoSmart&) = delete;
	BattleshipGameAlgoSmart& operator = (const BattleshipGameAlgoSmart&) = delete;

	//constructor
	BattleshipGameAlgoSmart()
	{
	}

	~BattleshipGameAlgoSmart()
	{
		if (playerBoard != nullptr)
			delete playerBoard;
		if (target != nullptr)
			delete target;
	}

	virtual void setBoard(int player, const char** board, int numRows, int numCols) override;
	virtual bool init(const std::string& path) override;
	virtual pair<int, int> attack() override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

private:
	void _markIrrelevant(int i, int j) const;
	bool _canAttack(int i, int j) const;
	pair<int, int> _getBestGuess() const;
};