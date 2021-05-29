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
	board = b;
	rows = _rowDimension;
	cols = _colDimension;

    uncoverGoal = (_rowDimension * _colDimension) - _totalMines;
	uncovered = 0;

	row_uncovered = _agentY;
	col_uncovered = _agentX;
	
	pair<int, int> firstTile(row_uncovered, col_uncovered);
	S.insert(firstTile);
};

Agent::Action MyAI::getAction( int number )
{
    // cout << "Effective Labels: " << endl;
    // for(auto it = effectiveLabel.cbegin(); it != effectiveLabel.cend(); ++it)
    // {
    //     std::cout << to_string(it->first.first + 1) + ", " + to_string(it->first.second + 1) << ": " << to_string(it->second) << "\n";
    // }

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

    // for (int i = board.size() - 1; i > -1; i--) {
	// 	vector<int> row = board[i];
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
        board[row_uncovered][col_uncovered] = number;

        pair<int, int> x(row_uncovered, col_uncovered);
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
                set<pair<int, int>> unmarkedTiles;
                set<pair<int,int>> markForDeletion;
				
				// bool newMark = true;
				// while (newMark) {
				// 	newMark = false;
				for (auto q : Q)
				{
					unmarkedTiles = getCoveredOrFlaggedTiles(q);

					// cout << "Checking AMN(q) for pair: " << endl;
					// cout << q.first + 1 << ", " << q.second << endl;

					if (board[q.first][q.second] == unmarkedTiles.size())
					{
						// cout << "For q: "<< endl;
						// cout << q.first + 1 << ", " << q.second + 1 << endl;
						for (auto tile: unmarkedTiles)
						{
							if (board[tile.first][tile.second] != -2)
							{
								mark(tile);
								// cout << "Marked" << tile.first + 1 << ", " << tile.second + 1 << endl;
								// newMark = true;
							}
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
				// }

				// cout << endl;
				// for (int i = board.size() - 1; i > -1; i--) {
				// 	vector<int> row = board[i];
				//     string rowString = "";
				//     for (int val: row) {
				//         if (val == -1){
				//             rowString += "-";
				//         }
				// 		else if (val == -2){
				//             rowString += "*";
				//         }
				//         else{
				//             rowString += to_string(val);
				//         }
				//         rowString += " ";
				//     }
				//     cout << rowString << endl;
				// }

				// for (auto q : Q) {
				// 	cout << "Pair: " << q.first << ", " << q.second << endl;
				// }
			}
			int x = 0;
			while (S.size() == 0)
			{
				if (x == 5) {
					break;
				}
				set<pair<int, int>> coveredTiles;
				for (auto q : Q) {
					// Find the uncovered tiles next to each q and update their probs
					coveredTiles = getCoveredTiles(q);
					for (auto tile : coveredTiles) {
						if (probabilities.count(tile) > 0) {
							// probabilities[tile] += 1.0 / (double) coveredTiles.size();
							probabilities[tile] += (double) effectiveLabel[q] / (double) coveredTiles.size();

						}
						else {
							// probabilities[tile] = 1.0 / (double) coveredTiles.size();
							probabilities[tile] = (double) effectiveLabel[q] / (double) coveredTiles.size();
						}
					}
				}
				// // Find the uncovered tile with the maximum probability and insert them into S
				double max = 0.0;
				for (auto const& tile : probabilities)
				{
					if(tile.second > max){
						max = tile.second;
					}
				}

				set<pair<int,int>> markForDeletion;
				// set<pair<int, int>> toUncover;
				int limit = 0;
				// for (auto const& tile : probabilities) { 
				// 	cout << tile.first.second + 1 << ", " << tile.first.first + 1 << ": " << tile.second << endl;
				// }
				for (auto const& tile : probabilities)
				{	
					// cout << tile.first.second + 1 << ", " << tile.first.first + 1 << ": " << tile.second << endl;
					if(tile.second == max){
						limit++;
						mark(tile.first);
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
						if (limit == 1) {
							break;
						}

						// UNCOMMENT UNDER FOR RANDOM GUESSING BETWEEN EQUAL PROBABILITIES

						// toUncover.insert(tile.first);
					}
				}

				// UNCOMMENT UNDER FOR RANDOM GUESSING BETWEEN EQUAL PROBABILITIES
 
				// if (toUncover.size() > 0) {
				// 	int r = rand() % toUncover.size();
				// 	int j = 0;
				// 	for (auto tile : toUncover) {
				// 		if (r == j) {
				// 			mark(tile);
				// 			for (auto q : Q)
				// 			{
				// 				if (effectiveLabel[q] == 0)
				// 				{
				// 					addUnmarkedNeighborsToS(q);
				// 					markForDeletion.insert(q);
				// 				}   
				// 			}
				// 			for (auto d : markForDeletion)
				// 			{
				// 				Q.erase(d);
				// 			}
				// 			markForDeletion.clear();
				// 		}
				// 		j++;
				// 	}
				// }
				
				probabilities.clear();			

				// for (auto const& tile : probabilities)
				// {
				// 	cout << "Key: " << tile.first.first + 1 << ", " << tile.first.second + 1 << " Value: " << tile.second << endl;
				// }

				// for (auto s : S) {
				// 	cout << "S Pair: " << s.first + 1 << ", " << s.second + 1 << endl;
				// }
				// Checking if S is still empty even after checking for flags
				//     This is where we implement the educated guess
				// cout << "NEEDED: Implement Educated Guess Functionality" << endl;
				x++;
			}
			// for (auto s : S) {
			// 	cout << "S Pair: " << s.first + 1 << ", " << s.second + 1 << endl;
			// }

			if (S.size() == 0) {
				int z = 0;
				int numSafeTiles = uncoverGoal - uncovered;
				set<pair<int, int>> toUncover;
				for (int i = board.size() - 1; i > -1; i--) {
					vector<int> row = board[i];
					for (int j = row.size() - 1; j > -1; j--) {
						if (row[j] == -1) {
							// z++;
							pair<int, int> x(i, j);
							toUncover.insert(x);
							// break;
							// if (z == y) {
							// 	break;
							// }
						}
					}
				}
				if (toUncover.size() == numSafeTiles) {
					S.insert(toUncover.begin(), toUncover.end());
				}
				else {
					S.insert(*toUncover.begin());
				}
			}
            if (S.size() != 0)
            {
				// cout << "_------------------------------" << endl;
				// for (int i = board.size() - 1; i > -1; i--) {
				// 	vector<int> row = board[i];
				// 	string rowString = "";
				// 	for (int val: row) {
				// 		if (val == -1){
				// 			rowString += "-";
				// 		}
				// 		else if (val == -2){
				// 			rowString += "*";
				// 		}
				// 		else{
				// 			rowString += to_string(val);
				// 		}
				// 		rowString += " ";
				// 	}
				// 	cout << rowString << endl;
				// }
                pair<int,int> coord = *S.begin();
                row_uncovered = coord.first;
                col_uncovered = coord.second;
                S.erase(S.begin());

                // Update number of tiles uncovered
                uncovered++;

                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> diff = end-start;
                total_time_elapsed += diff.count();
                return {UNCOVER, col_uncovered, row_uncovered};
            }
		}
	}


	if(uncovered != uncoverGoal){
		cout<<uncovered<<" "<<uncoverGoal<<endl;
		cout<<"FAIL"<<endl;
	}

	
	
	// for (int i = board.size() - 1; i > -1; i--) {
	// 	vector<int> row = board[i];
	// 	string rowString = "";
	// 	for (int val: row) {
	// 		if (val == -1){
	// 			rowString += "-";
	// 		}
	// 		else if (val == -2){
	// 			rowString += "*";
	// 		}
	// 		else{
	// 			rowString += to_string(val);
	// 		}
	// 		rowString += " ";
	// 	}
	// 	cout << rowString << endl;
	// }
	return {LEAVE,-1,-1};
}

set<pair<int, int>> MyAI::getCoveredTiles(pair<int, int> x)
{
    set<pair<int, int>> unmarkedTiles;
    int i = x.first;
    int j = x.second;

	// CHANGE THESE TO "&& board[][] < 0"

    if(j + 1 < board[i].size() && (board[i][j+1] == -1))
	{
		pair<int,int> right(i,j+1);
		unmarkedTiles.insert(right);
	}
	// i, j - 1
	if(j - 1 >= 0 && (board[i][j-1] == -1))
	{
		pair<int,int> left(i,j-1);
		unmarkedTiles.insert(left);
	}
	// i + 1, j
	if(i + 1 < board.size() && (board[i+1][j] == -1))
	{
		pair<int,int> down(i+1,j);
		unmarkedTiles.insert(down);
	}
	// i - 1, j
	if(i - 1 >= 0 && (board[i-1][j] == -1))
	{
		pair<int,int> up(i-1,j);
		unmarkedTiles.insert(up);
	}
	// i + 1, j - 1
	if(i + 1 < board.size() && j - 1 >= 0 && (board[i+1][j-1] == -1))
	{
		pair<int,int> downLeft(i+1,j-1);
		unmarkedTiles.insert(downLeft);
	}
	// i + 1, j + 1
	if(i + 1 < board.size() && j + 1 < board[i].size() && (board[i+1][j+1] == -1))
	{
		pair<int,int> downRight(i+1,j+1);
		unmarkedTiles.insert(downRight);
	}
	// i - 1, j - 1
	if(i - 1 >= 0 && j - 1 >= 0 && (board[i-1][j-1] == -1))
	{
		pair<int,int> upLeft(i-1,j-1);
        unmarkedTiles.insert(upLeft);
	}
	// i - 1, j + 1
	if(i - 1 >= 0 && j + 1 < board[i].size() && (board[i-1][j+1] == -1))
	{
		pair<int,int> upRight(i-1,j+1);
		unmarkedTiles.insert(upRight);
	}

    return unmarkedTiles;
}

set<pair<int, int>> MyAI::getCoveredOrFlaggedTiles(pair<int, int> x)
{
    set<pair<int, int>> unmarkedTiles;
    int i = x.first;
    int j = x.second;

	// CHANGE THESE TO "&& board[][] < 0"

    if(j + 1 < board[i].size() && (board[i][j+1] == -1 || board[i][j+1] == -2))
	{
		pair<int,int> right(i,j+1);
		unmarkedTiles.insert(right);
	}
	// i, j - 1
	if(j - 1 >= 0 && (board[i][j-1] == -1 || board[i][j-1] == -2))
	{
		pair<int,int> left(i,j-1);
		unmarkedTiles.insert(left);
	}
	// i + 1, j
	if(i + 1 < board.size() && (board[i+1][j] == -1 || board[i+1][j] == -2))
	{
		pair<int,int> down(i+1,j);
		unmarkedTiles.insert(down);
	}
	// i - 1, j
	if(i - 1 >= 0 && (board[i-1][j] == -1 || board[i-1][j] == -2))
	{
		pair<int,int> up(i-1,j);
		unmarkedTiles.insert(up);
	}
	// i + 1, j - 1
	if(i + 1 < board.size() && j - 1 >= 0 && (board[i+1][j-1] == -1 || board[i+1][j-1] == -2))
	{
		pair<int,int> downLeft(i+1,j-1);
		unmarkedTiles.insert(downLeft);
	}
	// i + 1, j + 1
	if(i + 1 < board.size() && j + 1 < board[i].size() && (board[i+1][j+1] == -1 || board[i+1][j+1] == -2))
	{
		pair<int,int> downRight(i+1,j+1);
		unmarkedTiles.insert(downRight);
	}
	// i - 1, j - 1
	if(i - 1 >= 0 && j - 1 >= 0 && (board[i-1][j-1] == -1 || board[i-1][j-1] == -2))
	{
		pair<int,int> upLeft(i-1,j-1);
        unmarkedTiles.insert(upLeft);
	}
	// i - 1, j + 1
	if(i - 1 >= 0 && j + 1 < board[i].size() && (board[i-1][j+1] == -1 || board[i-1][j+1] == -2))
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
        if (effectiveLabel.find(left) ==  effectiveLabel.end()) {
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
