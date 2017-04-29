#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include <queue> 
#include "Common.h"

using namespace std;



class BattleshipGameAlgoFile : public IBattleshipGameAlgo
{
public:
	int playerNum;
	Player playerName;
	BattleBoard* playerBoard = nullptr;
	string attackFilePath;
	queue<string> movesQueue;
	

	// Blocking Copy and Assignment
	BattleshipGameAlgoFile(const BattleshipGameAlgoFile&) = delete;
	BattleshipGameAlgoFile& operator = (const BattleshipGameAlgoFile&) = delete;

	//constructor
	BattleshipGameAlgoFile(Player playerName, string attackFilePath)
	{
		this->playerName = playerName;
		this->attackFilePath = attackFilePath;
		

	}
	~BattleshipGameAlgoFile()
	{
		if (playerBoard != nullptr)
			delete playerBoard;
		
	}

	virtual void setBoard(int player, const char** board, int numRows, int numCols) override;
	virtual bool init(const std::string& path) override;
	virtual std::pair<int, int> attack() override;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;
	/*
	*check if attack line is valid and contain indexes in range, one comma and doesn't contain
	*white spacecs at the begining or end else return False
	*
	*/
	bool isAttackLineValid(string line);
};