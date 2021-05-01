// ======================================================================
// FILE:        MyAI.hpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <utility>
#include <chrono>

using namespace std;

class MyAI : public Agent
{
public:
    static int count;

	MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

	Action getAction ( int number ) override;

	set<pair<int, int>> getCoveredOrFlaggedTiles(pair<int,int> x);

	void addUnmarkedNeighborsToS(pair<int,int> x);

	void mark(pair<int, int> y);

private:
	vector<vector<int>> board;
	map<pair<int, int>, int> effectiveLabel;
	set<pair<int,int>> S;
    set<pair<int,int>> Q;
	int rows;
	int cols;
	int uncovered;
	int uncoverGoal;
	int row_uncovered;
	int col_uncovered;
	double total_time_elapsed;
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
