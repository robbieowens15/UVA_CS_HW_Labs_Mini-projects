/*
Robert Owens (rao7utn)
traveling.cpp
11/17/2020
*/

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cfloat>

using namespace std;

#include "middleearth.h"

float computeDistance(MiddleEarth& me, const string& start, vector<string>& dests);
void printRoute(const string& start, const vector<string>& dests);

void travel(MiddleEarth& me, vector<string>& dests){
    vector<string> shortestPath = dests;
    float shortDistance = FLT_MAX;

    string start = dests.at(0);
    dests.at(0) = dests.at(dests.size()-1);
    dests.pop_back();

    sort(dests.begin(), dests.end());

    do{   
        float distance = computeDistance(me, start, dests);
        if(distance < shortDistance){
            shortDistance = distance;
            shortestPath = dests;
        }
    } while(next_permutation(dests.begin(), dests.end()));

    cout << "Minimun path has distance "<< shortDistance << ": ";
    printRoute(start, shortestPath);
}

int main(int argc, char** argv) {
    // check the number of parameters
    if (argc != 6) {
        cout << "Usage: " << argv[0] << " <world_height> <world_width> "
             << "<num_cities> <random_seed> <cities_to_visit>" << endl;
        exit(0);
    }

    // we'll assume the parameters are all well-formed
    int width = stoi(argv[1]);
    int height = stoi(argv[2]);
    int num_cities = stoi(argv[3]);
    int rand_seed = stoi(argv[4]);
    int cities_to_visit = stoi(argv[5]);

    // create the world, and select your itinerary
    MiddleEarth me(width, height, num_cities, rand_seed);
    vector<string> dests = me.getItinerary(cities_to_visit);

    // TODO: YOUR CODE HERE
    travel(me, dests);
    return 0;
}

// This method will compute the full distance of the cycle that starts
// at the 'start' parameter, goes to each of the cities in the dests
// vector IN ORDER, and ends back at the 'start' parameter.
float computeDistance(MiddleEarth& me, const string& start, vector<string>& dests) {
    // TODO: YOUR CODE HERE
    float sum = 0.0;
    sum += me.getDistance(start, dests.at(0));
    for(int i = 1; i < dests.size(); i++){
        sum += me.getDistance(dests.at(i-1),dests.at(i));
    }
    sum += me.getDistance(dests.at(dests.size()-1), start);
    return sum;
}

// This method will print the entire route, starting and ending at the
// 'start' parameter.
// The output should be similar to:
// Erebor -> Khazad-dum -> Michel Delving -> Bree -> Cirith Ungol -> Erebor
void printRoute(const string& start, const vector<string>& dests) {
    // TODO: YOUR CODE HERE
    cout << start + " -> ";
    for(int i = 0; i < dests.size(); i++){
        cout << dests.at(i) + " -> ";
    }
    cout << start << endl;
}