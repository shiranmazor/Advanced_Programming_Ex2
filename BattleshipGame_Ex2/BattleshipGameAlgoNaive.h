#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "BattleBoard.h"
#include "Common.h"

using namespace std;

class BattleshipGameAlgoNaive : public IBattleshipGameAlgo
{
public:
	Player playerName;
	BattleBoard* playerBoard = nullptr;

	// Blocking Copy and Assignment
	BattleshipGameAlgoNaive(const BattleshipGameAlgoNaive&) = delete;
	BattleshipGameAlgoNaive& operator = (const BattleshipGameAlgoNaive&) = delete;
};