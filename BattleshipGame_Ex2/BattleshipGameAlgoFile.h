#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include <queue> 

using namespace std;

#define MyHitMark '*'
#define OpHitMark '#'
#define MyMissMark '~'
#define OpMissMark '^'
#define MySinkMark '!'
#define OpSinkMark '?'

class BattleshipGameAlgoFile : public IBattleshipGameAlgo
{
public:
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
		//load attack file to file handle assuming file path is correct:
		ifstream attackFile;
		attackFile.open(attackFilePath);
		if (attackFile.fail())
		{
			std::cout << "Error while open internal file" << std::endl;
			attackFile.close();
		}
		else //load all attack lines to movesQueue
		{
			string line;
			while (getline(attackFile, line))
			{
				movesQueue.push(line);
			}
			attackFile.close();
		}

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