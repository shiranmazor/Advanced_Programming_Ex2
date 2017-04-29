#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue> 
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include "Common.h"

using namespace std;

class BattleshipGameAlgoNaive : public IBattleshipGameAlgo
{
public:
	Player playerName;
	BattleBoard* playerBoard = nullptr;
	queue<pair<int,int>> attackQueue;

	// Blocking Copy and Assignment
	BattleshipGameAlgoNaive(const BattleshipGameAlgoNaive&) = delete;
	BattleshipGameAlgoNaive& operator = (const BattleshipGameAlgoNaive&) = delete;

	//constructor
	BattleshipGameAlgoNaive()
	{

	}
	~BattleshipGameAlgoNaive()
	{
		if (playerBoard != nullptr)
			delete playerBoard;
	}

	virtual void setBoard(int player, const char** board, int numRows, int numCols) override;
	virtual bool init(const std::string& path) override;
	virtual std::pair<int, int> attack() override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

};