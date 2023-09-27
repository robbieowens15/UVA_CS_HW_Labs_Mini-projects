/*
Robert Owens (rao7utn)
10/12/2020
Lab 6 Pre-lab
wordPuzzle.cpp
*/

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include "timer.h"
#include "Hashtable.h"

using namespace std;

#define MAXROWS 500
#define MAXCOLS 500
char grid[MAXROWS][MAXCOLS];
Hashtable dictonary = NULL;

/** This function will read in a grid file, as per the format in the
 * CS 2150 lab 6 document, into a global grid[][] array.  It uses C++
 * file streams, and thus requires the the <fstream> #include header.
 *
 * @return true or false, depending on whether the file was
 *         successfully opened.
 * @param filename The file name to read in -- it's assumed to be in
 *                 the file format described in the lab document.
 * @param rows The number of rows as specified in the input file;
 *             as this is a reference, it is set by the function.
 * @param cols The number of columns as specified in the input file;
 *             as this is a reference, it is set by the function.
 */
bool readInGrid(string filename, int& rows, int& cols) {
    // try to open the file
    ifstream file(filename);
    // upon an error, return false
    if (!file.is_open()) {
        return false;
    }

    // first comes the number of rows
    file >> rows;

    // then the columns
    file >> cols;

    // and finally the grid itself
    string data;
    file >> data;

    // close the file
    file.close();

    // convert the string read in to the 2-D grid format into the
    // grid[][] array.
    // In the process, we'll print the grid to the screen as well.
    int pos = 0; // the current position in the input data
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            grid[r][c] = data[pos++];
        }
    }
    return true;
}

string getWordInGrid (int startRow, int startCol, int dir, int len,
                      int numRows, int numCols) {
    // the static-ness of this variable prevents it from being
    // re-declared upon each function invocation.  It also prevents it
    // from being deallocated between invocations.  It's probably not
    // good programming practice, but it's an efficient means to return
    // a value.
    static string output;
    output.clear(); // Since it's static we need to clear it
    output.reserve(256); // Can't set capacity in the constructor so do it the first time here

    // the position in the output array, the current row, and the
    // current column
    int r = startRow, c = startCol;
    // iterate once for each character in the output
    for (int i = 0; i < len; i++) {
        // if the current row or column is out of bounds, then break
        if (c >= numCols || r >= numRows || r < 0 || c < 0) {
            break;
        }

        // set the next character in the output array to the next letter
        // in the grid
        output += grid[r][c];

        // move in the direction specified by the parameter
        switch (dir) { // assumes grid[0][0] is in the upper-left
            case 0:
                r--;
                break; // north
            case 1:
                r--;
                c++;
                break; // north-east
            case 2:
                c++;
                break; // east
            case 3:
                r++;
                c++;
                break; // south-east
            case 4:
                r++;
                break; // south
            case 5:
                r++;
                c--;
                break; // south-west
            case 6:
                c--;
                break; // west
            case 7:
                r--;
                c--;
                break; // north-west
        }
    }

    return output;
}

int* findRowsAndCols(string filename){
	static int vals[2];
	string numAsString = "";
	for (int i = 0; i < filename.length(); i++){
		if(filename.at(i) == 'x'){
			//first number
			vals[0] = stoi(numAsString);
			numAsString = "";
		}
		else if(filename.at(i) == '.') {
			//second number
			vals[1] = stoi(numAsString);
			break;
		}
		else{
			//build number
			numAsString += filename.at(i);
		}
	}
	return vals;
}

int numWordsInFile(string filename){
    // try to open the file
    ifstream file(filename);
    // upon an error, return false
    if (!file.is_open()) {
        return -1;
    }

    /*
	This method for counting the number of files in a txt file is not my IP.
	The method comes from Stack Overflow here: https://stackoverflow.com/questions/3072795/how-to-count-lines-of-a-file-in-c
	Date Accessed: 10/12/2020
    */
    int temp = count(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), '\n');

    // close the file
    file.close();
    return temp;
}

void readFileToDictonary(string filename, int size){
	// try to open the file
    ifstream file(filename);
    // upon an error, return false
    if (!file.is_open()) {
        cout << "No Such File" << endl;
        exit(-1);
    }

    string line;
  	ifstream myfile (filename);
  	if (myfile.is_open()){
    	while ( getline (myfile, line)){
    		if (line.length() >= 3){
    			dictonary.insert(line);
    		}	
    	}
    	myfile.close();
  	}

  	else cout << "Unable to open file: " + filename;

    // close the file
    file.close();
}

void formatedOutput(string word, int dir, int row, int col){
	string direction = "";
    switch (dir) {
		case 0:
			direction = "N ";
			break; // north
		case 1:
			direction = "NE";
			break; // north-east
		case 2:
			direction = "E ";
			break; // east
		case 3:
			direction = "SE";
			break; // south-east
		case 4:
			direction = "S ";
			break; // south
		case 5:
			direction = "SW";
			break; // south-west
		case 6:
			direction = "W ";
			break; // west
		case 7:
			direction = "NW";
		break; // north-west
	}
	cout << direction + "(" << row << ", " << col << "):\t" + word << endl;
}

void searchForWords(int rows, int cols){
	int counter = 0;
	for (int row = 0; row < rows; row ++){
		for (int col = 0; col < cols; col++){
			for (int direction = 0; direction < 8; direction++){
				set<string> uniqueWords;
				for (int len = 22; len >= 3; len--){ //Max length is 22, min is 3
					string test = getWordInGrid(row, col, direction, len, rows, cols);
					if(dictonary.find(test) &&
						uniqueWords.find(test) == uniqueWords.end()){
						formatedOutput(test, direction, row, col);
						counter++;
						uniqueWords.insert(test);
					}
				}
			}
		}
	}
	cout << counter << " words found" << endl;
}

int main (int argc, char *argv[]){
	timer t;
    t.start();

	if (argc < 3){
		cout << "Error: 2 command-line arguments needed for this script" << endl;
		exit(-1);
	}

	//argv[1] is a dictonary file (list of valid words)
	//argv[2] is also a file (the wordpuzzle)

	unsigned int numWords = numWordsInFile(argv[1]);
	dictonary = Hashtable(numWords);
	readFileToDictonary(argv[1], numWords);

	 // to hold the number of rows and cols in the input file
    int rows, cols;

    // attempt to read in the file
    //bool result = readInGrid(argv[2], rows, cols);

    bool result = readInGrid(argv[2], rows, cols);
    // if there is an error, report it
    if (!result) {
        cout << "Error reading in file!" << endl;
        return 1;
    }

    // Get a word (of length 10), starting at position (2,2) in the
    // array, in each of the 8 directions

    searchForWords(rows, cols);
    t.stop();
    //cout << t.getTime() << endl; //time in microseconds (10^-6) //Uncomment when testing speed
    return 0;
}
