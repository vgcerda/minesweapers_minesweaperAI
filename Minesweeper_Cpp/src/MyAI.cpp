// ======================================================================
// FILE:        MyAI.cpp
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

#include "MyAI.hpp"

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
	total_time_elapsed = 0.0;
	vector<vector<int>> b(_rowDimension, vector<int>(_colDimension, -1));
	rows = _rowDimension;
	cols = _colDimension;
	board = b;

    	uncoverGoal = (_rowDimension * _colDimension) - totalMines;

	uncovered = 0;

	x_uncovered = _agentX;
	y_uncovered = _agentY;

	pair<int, int> firstTile(_agentX, _agentY);
	S.insert(firstTile);
};

Agent::Action MyAI::getAction( int number )
{
	if(total_time_elapsed > 290.0)
	{
		// Make random moves
		return{actions[rand() % 4], rand() % rows, rand() % cols};
	}
	else
	{
		auto start = std::chrono::system_clock::now();
		board[x_uncovered][y_uncovered] = number;
        pair<int, int> x_y(x_uncovered, y_uncovered);
        effectiveLabel[x_y] = number;

		if (uncoverGoal != uncovered)
		{
			if(S.size() == 0)
			{
				findToUncover();
			}
			if(S.size() != 0)
			{
				// Get UNCOVER coord from S
				pair<int,int> coord = *S.begin();
				x_uncovered = coord.first;
				y_uncovered = coord.second;

				S.erase(S.begin());

				// Update number of tiles uncovered
				uncovered++;

				// Update uncovered tiles set
				pair<int, int> c(x_uncovered, y_uncovered);
				uncoveredSet.insert(c);

				auto end = std::chrono::system_clock::now();
				std::chrono::duration<double> diff = end-start;
				total_time_elapsed += diff.count();
				return {UNCOVER, coord.first, coord.second};
			}
		}
	}
	return {LEAVE,-1,-1};
}

void MyAI::findToUncover()
{
	for(auto coord : uncoveredSet)
	{
        // cout << to_string(coord.first) + " " +  to_string(coord.second);
		int i = coord.first;
		int j = coord.second;
		pair<int, int> c(i,j);
		if(board[i][j] == 0 && visitedZeros.find(coord) == visitedZeros.end())
		{
			// i , j + 1
			if(j + 1 < board[i].size())
			{
				pair<int,int> right(i,j+1);
				if(uncoveredSet.find(right) == uncoveredSet.end())
				{
					S.insert(right);
				}
			}
			// i, j - 1
			if(j - 1 >= 0)
			{
				pair<int,int> left(i,j-1);
				if(uncoveredSet.find(left) == uncoveredSet.end())
				{
					S.insert(left);
				}
			}
			// i + 1, j
			if(i + 1 < board.size())
			{
				pair<int,int> down(i+1,j);
				if(uncoveredSet.find(down) == uncoveredSet.end())
				{
					S.insert(down);
				}
			}
			// i - 1, j
			if(i - 1 >= 0)
			{
				pair<int,int> up(i-1,j);
				if(uncoveredSet.find(up) == uncoveredSet.end())
				{
					S.insert(up);
				}
			}
			// i + 1, j - 1
			if(i + 1 < board.size() && j - 1 >= 0)
			{
				pair<int,int> downLeft(i+1,j-1);
				if(uncoveredSet.find(downLeft) == uncoveredSet.end())
				{
					S.insert(downLeft);
				}
			}
			// i + 1, j + 1
			if(i + 1 < board.size() && j + 1 < board[i].size())
			{
				pair<int,int> downRight(i+1,j+1);
				if(uncoveredSet.find(downRight) == uncoveredSet.end())
				{
					S.insert(downRight);
				}
			}
			// i - 1, j - 1
			if(i - 1 >= 0 && j - 1 >= 0)
			{
				pair<int,int> upLeft(i-1,j-1);
				if(uncoveredSet.find(upLeft) == uncoveredSet.end())
				{
					S.insert(upLeft);
				}
			}
			// i - 1, j + 1
			if(i - 1 >= 0 && j + 1 < board[i].size())
			{
				pair<int,int> upRight(i-1,j+1);
				if(uncoveredSet.find(upRight) == uncoveredSet.end())
				{
					S.insert(upRight);
				}
			}
			visitedZeros.insert(c);
		}
	}

    // if (S.size() == 0)
    // {

    // }
}
