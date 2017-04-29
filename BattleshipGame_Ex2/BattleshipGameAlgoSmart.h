#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include "Common.h"

using namespace std;

class BattleshipGameAlgoSmart : public IBattleshipGameAlgo
{
public:
	Player playerName;
	BattleBoard* playerBoard = nullptr;

	// Blocking Copy and Assignment
	BattleshipGameAlgoSmart(const BattleshipGameAlgoSmart&) = delete;
	BattleshipGameAlgoSmart& operator = (const BattleshipGameAlgoSmart&) = delete;

	virtual void setBoard(int player, const char** board, int numRows, int numCols) override;
	virtual bool init(const std::string& path) override;
	virtual std::pair<int, int> attack() override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

};