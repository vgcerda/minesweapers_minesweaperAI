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
#include <pair>;
MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
	vector<vector<int>> b(_rowDimension, vector<int> (_colDimension));
	board = b;
	uncovered = 0;
	x_uncovered = _agentX;
	y_uncovered = _agentY;
	uncoverGoal = (_rowDimension * _colDimension) - totalMines;
};

Agent::Action MyAI::getAction( int number )
{
	board[x_uncovered][y_uncovered] = number;
	uncovered++; 
	if(uncoverGoal == uncovered)
	{
		return {LEAVE,-1,-1};
	}
	else
	{
		if(queue.size() == 0)
		{
			findToUncover();
		}
		pair<int,int> coor = queue[queue.size()-1];
		queue.pop_back();
		return {UNCOVER, coor.first, coor.second};
	}
}

void MyAI::findToUncover()
{
	for(auto coord : uncoveredSet)
	{
		int i = coord.first;
		int j = coord.second;
		pair<int, int> c(i,j);
		if(board[i][j] == 0 && visitedZeros.find(c) != visitedZeros.end())
		{
			// i , j + 1
			if(j + 1 < board[i].size())
			{
				pair<int,int> right(i,j+1);
				if(uncoveredSet.find(right) != uncoveredSet.end())
				{
					queue.push_back(right);
				}
			}
			// i, j - 1
			if(j - 1 > 0)
			{
				pair<int,int> left(i,j-1);
				if(uncoveredSet.find(left) != uncoveredSet.end())
				{
					queue.push_back(left);
				}
			}
			// i + 1, j
			if(i + 1 < board.size())
			{
				pair<int,int> down(i+1,j);
				if(uncoveredSet.find(down) != uncoveredSet.end())
				{
					queue.push_back(down);
				}
			}
			// i - 1, j
			if(i - 1 > 0)
			{
				pair<int,int> up(i-1,j);
				if(uncoveredSet.find(up) != uncoveredSet.end())
				{
					queue.push_back(up);
				}
			}
			// i + 1, j - 1
			if(i + 1 < board.size() && j - 1 > 0)
			{
				pair<int,int> downLeft(i+1,j-1);
				if(uncoveredSet.find(downLeft) != uncoveredSet.end())
				{
					queue.push_back(downLeft);
				}
			}
			// i + 1, j + 1
			if(i + 1 < board.size() && j + 1 < board[i].size())
			{
				pair<int,int> downRight(i+1,j+1);
				if(uncoveredSet.find(downRight) != uncoveredSet.end())
				{
					queue.push_back(downRight);
				}
			}
			// i - 1, j - 1
			if(i - 1 > 0 && j - 1 > 0)
			{
				pair<int,int> upLeft(i-1,j-1);
				if(uncoveredSet.find(upLeft) != uncoveredSet.end())
				{
					queue.push_back(upLeft);
				}
			}
			// i - 1, j + 1
			if(i - 1 > 0 && j + 1 < board[i].size())
			{
				pair<int,int> upRight(i-1,j+1);
				if(uncoveredSet.find(upRight) != uncoveredSet.end())
				{
					queue.push_back(upRight);
				}
			}
			visitedZeros.insert(c);
		}
	}
}
