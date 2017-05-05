#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include "Common.h"

using namespace std;

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
	set<pair<int, int>> irrelevantCells;
	int hostileShipsNum = -1;
	int counter = 0;

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
	virtual std::pair<int, int> attack() override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	bool _canAttack(int i, int j);
};