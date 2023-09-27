/*
Robert Owens (rao7utn)
middleearth.cpp
11/15/2020
*/

#ifndef MIDDLEEARTH_H
#define MIDDLEEARTH_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <random>

using namespace std;

// see the comments in the lab11 write-up, or in middleearth.cpp

class MiddleEarth {
public:
    /**
     * \brief   Constructor of middle earth
     * 
     * This Constuctor will generate a MiddleEarth object
     *
     * \param   xsize is the width of the map
     * \param   ysize is the height of the map
     * \param   num_cities is the number of cities in the map
     * \param   seed is the sudo-random seed used to generate a random list of cities
     *
     * \return  a MiddleEarth object instance
     */
    MiddleEarth(int xsize, int ysize, int num_cities, int seed);

    /**
     * \brief   Prints the starting cities and cities to vist on a trip
     *
     * From the randomly generated list of cities, an itinerary is created
     * From which the shorted path shall be generated
     */
    void print();

    /**
     * \brief   prints out a 2-d table of all cities and the distances between them
     */
    void printTable();

    /**
     * \brief       Calulates the distnace between 2 cities
     * 
     * \attention   These parameters are passed by value not by reference
     * \param       city1 is the first city
     * \param       city2 is the second city
     * 
     * \return      a float which represents the cartiaian distance between the two cities
     */
    float getDistance(const string& city1, const string& city2);

    /**
     * \brief   Getter for the itinerary
     *
     * \param   length an unsigned int to choose which partion of the itinerary to return (0 to length-1)
     */
    vector<string> getItinerary(unsigned int length);

private:
    int num_city_names, xsize, ysize;
    unordered_map<string, float> xpos, ypos;
    vector<string> cities;
    unordered_map<string, unordered_map<string, float>> distances;

    mt19937 gen; // Mersenne-Twister random number engine
};

#endif