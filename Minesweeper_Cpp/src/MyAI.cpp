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
int MyAI::count = 0;

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    count++;
    // cout << count << endl; 
	total_time_elapsed = 0.0;
	vector<vector<int>> b(_rowDimension, vector<int>(_colDimension, -1));
	rows = _rowDimension;
	cols = _colDimension;
	board = b;

    uncoverGoal = (_rowDimension * _colDimension) - totalMines;

	uncovered = 1;

	x_uncovered = _agentX;
	y_uncovered = _agentY;

	pair<int, int> firstTile(_agentX, _agentY);
	S.insert(firstTile);
};

Agent::Action MyAI::getAction( int number )
{
    // cout << "Effective Labels: " << endl;
    // for(auto it = effectiveLabel.cbegin(); it != effectiveLabel.cend(); ++it)
    // {
    //     std::cout << to_string(it->first.first + 1) + ", " + to_string(it->first.second + 1) << ": " << to_string(it->second) << "\n";
    // }
    // cout << "Hello" << endl;

    // cout << "Pairs in S:" << endl;
    // for (auto p: S)
    // {
    //     cout << p.first + 1 << ", " << p.second + 1<< "; ";
    // }
    // cout << endl;

    // cout << "Pairs in Q:" << endl;
    // for (auto p: Q)
    // {
    //     cout << p.first + 1 << ", " << p.second + 1 << "; ";
    // }
    // cout << endl;

    // for (vector<int> row: board) {
    //     string rowString = "";
    //     for (int val: row) {
    //         if (val == -1){
    //             rowString += "-";
    //         }
    //         else{
    //             rowString += to_string(val);
    //         }
    //         rowString += " ";
    //     }
    //     cout << rowString << endl;
    // }

	if(total_time_elapsed > 290.0)
	{
		// Make random moves
		return{UNCOVER, rand() % rows, rand() % cols};
	}
	else
	{
		auto start = std::chrono::system_clock::now();
        board[x_uncovered][y_uncovered] = number;
        pair<int, int> x(x_uncovered, y_uncovered);
        if (effectiveLabel.find(x) != effectiveLabel.end())
        {
            effectiveLabel[x] += number;
        }
        else
        {
            effectiveLabel.insert({x, number});
        }

		if (effectiveLabel[x] == 0)
		{
			addUnmarkedNeighborsToS(x);
		}
		else {
			Q.insert(x);
		}

		if (uncoverGoal != uncovered)
		{
            if(S.size() == 0)
			{
				// findToUncover();
                // for q ∈ Q do
                //     if isAMN(q) = True then
                //         for y ∈ Unmarked-Neighbors(q) do
                //             mark(y)
                //         end for
                //         Q.remove(q)
                //     end if
                // end for
                // for q ∈ Q do
                //     if isAFN(q) = True then
                //         S ← S ∪ Unmarked-Neighbors(q)
                //         Q.remove(q)
                //     end if
                // end for

                set<pair<int, int>> unmarkedTiles;
                set<pair<int,int>> markForDeletion;
                for (auto q : Q)
                {
                    unmarkedTiles = getUnmarkedTiles(q);
                    if (board[q.first][q.second] == unmarkedTiles.size())
                    {
                        for (auto tile: unmarkedTiles)
                        {
                            if (board[tile.first][tile.second] != -2)
                            {
                                // cout << "Flag: " << tile.first << ", " << tile.second << endl;
                                mark(tile);
                            }
                            // mark function flags the tile and updates the effectiveLabel values
                        }
                        markForDeletion.insert(q);
                    }
                }

                for (auto d : markForDeletion)
                {
                    Q.erase(d);
                }

                markForDeletion.clear();

                for (auto q : Q)
                {
                    if (effectiveLabel[q] == 0)
                    {
                        addUnmarkedNeighborsToS(q);
                        markForDeletion.insert(q);
                    }   
                }

                for (auto d : markForDeletion)
                {
                    Q.erase(d);
                }

                markForDeletion.clear();

				/////////////////////////////////////
				// Select Random square if s is empty 
				/////////////////////////////////////
			}
            if (S.size() != 0)
            {
                // Pop x from S queue, check if x is a 0, if it is, add neighbors to S, if not, add x to Q.
                // Get UNCOVER coord from S
                pair<int,int> coord = *S.begin();
                x_uncovered = coord.first;
                y_uncovered = coord.second;
                S.erase(S.begin());
                // Update number of tiles uncovered
                uncovered++;

                // // Update uncovered tiles set
                // pair<int, int> c(x_uncovered, y_uncovered);
                // uncoveredSet.insert(c);

                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> diff = end-start;
                total_time_elapsed += diff.count();
                return {UNCOVER, coord.first, coord.second};
            }
		}
	}
	return {LEAVE,-1,-1};
}

set<pair<int, int>> MyAI::getUnmarkedTiles(pair<int, int> x)
{
    set<pair<int, int>> unmarkedTiles;
    int i = x.first;
    int j = x.second;

    if(j + 1 < board[i].size() && board[i][j+1] < 0)
	{
		pair<int,int> right(i,j+1);
		unmarkedTiles.insert(right);
	}
	// i, j - 1
	if(j - 1 >= 0 && board[i][j-1] < 0)
	{
		pair<int,int> left(i,j-1);
		unmarkedTiles.insert(left);
	}
	// i + 1, j
	if(i + 1 < board.size() && board[i+1][j] < 0)
	{
		pair<int,int> down(i+1,j);
		unmarkedTiles.insert(down);
	}
	// i - 1, j
	if(i - 1 >= 0 && board[i-1][j] < 0)
	{
		pair<int,int> up(i-1,j);
		unmarkedTiles.insert(up);
	}
	// i + 1, j - 1
	if(i + 1 < board.size() && j - 1 >= 0 && board[i+1][j-1] < 0)
	{
		pair<int,int> downLeft(i+1,j-1);
		unmarkedTiles.insert(downLeft);
	}
	// i + 1, j + 1
	if(i + 1 < board.size() && j + 1 < board[i].size() && board[i+1][j+1] < 0)
	{
		pair<int,int> downRight(i+1,j+1);
		unmarkedTiles.insert(downRight);
	}
	// i - 1, j - 1
	if(i - 1 >= 0 && j - 1 >= 0 && board[i-1][j-1] < 0)
	{
		pair<int,int> upLeft(i-1,j-1);
        unmarkedTiles.insert(upLeft);
	}
	// i - 1, j + 1
	if(i - 1 >= 0 && j + 1 < board[i].size() && board[i-1][j+1] < 0)
	{
		pair<int,int> upRight(i-1,j+1);
		unmarkedTiles.insert(upRight);
	}

    return unmarkedTiles;
}

void MyAI::mark(pair<int, int> y)
{
    int i = y.first;
    int j = y.second;
    board[i][j] = -2;

	if(j + 1 < board[i].size() && board[i][j + 1] > -2)
	{
        pair<int, int> right(i, j+1);
        // if (eff)
        if (effectiveLabel.find(right) == effectiveLabel.end()) {
            effectiveLabel[right] = 0;
        }
        effectiveLabel[right] -= 1; 
        
	}
	// i, j - 1
	if(j - 1 >= 0 && board[i][j - 1] > -2)
	{
		pair<int,int> left(i,j-1);
        if (effectiveLabel.find(left) == effectiveLabel.end()) {
            effectiveLabel[left] = 0;
        }
        effectiveLabel[left] -= 1; 
	}
	// i + 1, j
	if(i + 1 < board.size() && board[i+1][j] > -2)
	{
		pair<int,int> down(i+1,j);
        if (effectiveLabel.find(down) == effectiveLabel.end()) {
            effectiveLabel[down] = 0;
        }
        effectiveLabel[down] -= 1; 
	}
	// i - 1, j
	if(i - 1 >= 0 && board[i-1][j] > -2)
	{
		pair<int,int> up(i-1,j);
		if (effectiveLabel.find(up) == effectiveLabel.end()) {
            effectiveLabel[up] = 0;
        }
        effectiveLabel[up] -= 1; 
	}
	// i + 1, j - 1
	if(i + 1 < board.size() && j - 1 >= 0 && board[i + 1][j - 1] > -2)
	{
		pair<int,int> downLeft(i+1,j-1);
		if (effectiveLabel.find(downLeft) == effectiveLabel.end()) {
            effectiveLabel[downLeft] = 0;
        }
        effectiveLabel[downLeft] -= 1; 
	}
	// i + 1, j + 1
	if(i + 1 < board.size() && j + 1 < board[i].size() && board[i + 1][j + 1] > -2)
	{
		pair<int,int> downRight(i+1,j+1);
		if (effectiveLabel.find(downRight) == effectiveLabel.end()) {
            effectiveLabel[downRight] = 0;
        }
        effectiveLabel[downRight] -= 1; 
	}
	// i - 1, j - 1
	if(i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] > -2)
	{
		pair<int,int> upLeft(i-1,j-1);
		if (effectiveLabel.find(upLeft) == effectiveLabel.end()) {
            effectiveLabel[upLeft] = 0;
        }
        effectiveLabel[upLeft] -= 1; 
	}
	// i - 1, j + 1
	if(i - 1 >= 0 && j + 1 < board[i].size() && board[i - 1][j + 1] > -2)
	{
		pair<int,int> upRight(i-1,j+1);
		if (effectiveLabel.find(upRight) == effectiveLabel.end()) {
            effectiveLabel[upRight] = 0;
        }
        effectiveLabel[upRight] -= 1; 
	}
}

void MyAI::addUnmarkedNeighborsToS(pair<int,int> x)
{
	int i = x.first;
	int j = x.second;
	// i , j + 1
	if(j + 1 < board[i].size() && board[i][j+1] == -1)
	{
		pair<int,int> right(i,j+1);
		S.insert(right);
	}
	// i, j - 1
	if(j - 1 >= 0 && board[i][j-1] == -1)
	{
		pair<int,int> left(i,j-1);
		S.insert(left);
	}
	// i + 1, j
	if(i + 1 < board.size() && board[i+1][j] == -1)
	{
		pair<int,int> down(i+1,j);
		S.insert(down);
	}
	// i - 1, j
	if(i - 1 >= 0 && board[i-1][j] == -1)
	{
		pair<int,int> up(i-1,j);
		S.insert(up);
	}
	// i + 1, j - 1
	if(i + 1 < board.size() && j - 1 >= 0 && board[i+1][j-1] == -1)
	{
		pair<int,int> downLeft(i+1,j-1);
		S.insert(downLeft);
	}
	// i + 1, j + 1
	if(i + 1 < board.size() && j + 1 < board[i].size() && board[i+1][j+1] == -1)
	{
		pair<int,int> downRight(i+1,j+1);
		S.insert(downRight);
	}
	// i - 1, j - 1
	if(i - 1 >= 0 && j - 1 >= 0 && board[i-1][j-1] == -1)
	{
		pair<int,int> upLeft(i-1,j-1);
		S.insert(upLeft);
	}
	// i - 1, j + 1
	if(i - 1 >= 0 && j + 1 < board[i].size() && board[i-1][j+1] == -1)
	{
		pair<int,int> upRight(i-1,j+1);
		S.insert(upRight);
	}
    // cout << "addUnmarkedNeighborsToS:Pairs in S:" << endl;
    // for (auto p: S)
    // {
    //     cout << p.first + 1 << ", " << p.second + 1<< "; ";
    // }
    // cout << endl;
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
