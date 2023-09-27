/*
Robert Owens (rao7utn)
puzzleSolver.cpp
11/18/2020
*/

#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>

using namespace std;

/**
 * \brief Swap Method for switching two tiles
 * This method will swap the postions of two tiles
 * the positions are respresented as postions in the string
 *
 * \param str the string to preform the character (tile) swap on
 * \remark Where index is the position in the string (str.at(index))
 * \param index1 the first index to be swapped
 * \param index2 the second index to be swapped
 * \return The new updated str representing the board state
 */
string swap(string str, unsigned int index1, unsigned int index2){
	char temp = str.at(index1);
	str.at(index1) = str.at(index2);
	str.at(index2) = temp;
	return str;
}

/**
 * \brief Generate the neighboring nodes of the graph
 * This method creates the 1st order neightbors to a provided
 * node. Nodes are represnted as 9 character strings
 * Will attempt to flip the hole tile ("0") up, down, right, left if possible
 *
 * \param curState the start from which to generate neightbors from
 * \return a vector of strings with all possible neighbors
 */
vector<string> genNeighbors(string curState){
	unsigned int hole = curState.find("0");

	vector<string> edgeNodes = vector<string>();

	//slide up
	if (!(hole < 3)){
		edgeNodes.push_back(swap(curState, hole, hole-3));
	}
	//side down
	if (!(hole > 5)){
		edgeNodes.push_back(swap(curState, hole, hole+3));
	}
	//side left
	if (!(hole%3 == 0)){
		edgeNodes.push_back(swap(curState, hole, hole-1));
	}
	//side right
	if (!((hole+1)%3 == 0)){
		edgeNodes.push_back(swap(curState, hole, hole+1));
	}
	return edgeNodes;
}


/**
 * \brief A Bread First Search Algorithm to find the shorted possible path to the finish state
 *
 * \param initState is the inital state of the board
 * \return a string representation of the shortest possible number of swaps to reach
 * the final state, or the string "IMMPOSSIBLE" if the final state is not in the graph
 */
string solver(string& initState){
	unordered_map<string, unsigned int> stateToDist = unordered_map<string, unsigned int>();

	unsigned int distance = 0;
	queue<string> q;
	q.push(initState);
	stateToDist[initState] = distance;

	while(q.size() > 0){
		string curState = q.front();
		distance = stateToDist[curState] + 1;
		q.pop();

		if(curState == "123456780"){
			return to_string(stateToDist[curState]);
		}

		vector<string> edgeNodes = genNeighbors(curState);
		for(int i = 0; i < edgeNodes.size(); i++){
			if(stateToDist.count(edgeNodes.at(i)) == 0){
				//Not yet visited
				q.push(edgeNodes.at(i));
				stateToDist[edgeNodes.at(i)] = distance;
			}
		}
	}
	return "IMPOSSIBLE";
}

/**
 * \brief reads in the game board and formats it as a string
 * \return the string representing the inital state of the board
 */
string readInput(){
	cout << "Enter puzzle" << endl;

	string input = "";
	for(int i = 0; i < 9; i ++){
		string temp;
		cin >> temp;
		input += temp;
	}
	return input;
}

/**
 * \brief Runs the puzzleSolver program
 */
int main(){
	string initState = readInput();
	cout << "Solving puzzle" << endl;
	cout << solver(initState) << endl;
	return 0;
}