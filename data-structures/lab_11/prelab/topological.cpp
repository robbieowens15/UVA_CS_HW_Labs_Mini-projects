/*
Robert Owens (rao7utn)
topoligcal.cpp
11/16/2020
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <unordered_set>
#include <deque>
#include "classnode.h"
using namespace std;

/**
 * \brief prints the whole graph
 * \remark useful for debugging
 */
void printGraph(vector<ClassNode*> verticies){
    for (int i = 0; i < verticies.size(); i++){
        verticies.at(i)->print();
    }
}

/**
 * \brief   Sorts the vetricies of the graph lexographically
 */
void bubbleSort(vector<ClassNode*>& verticies){
    for(int i = 0; i < verticies.size(); i++){
        for(int j = 1; j < verticies.size() - i; j++){
            if(verticies.at(j-1)->getName() > verticies.at(j)->getName()){
                ClassNode* temp = verticies.at(j-1);
                verticies.at(j-1) = verticies.at(j);
                verticies.at(j) = temp;
            }
        }
    }
}

/**
 * Required for the printTopSort() to work
 */
void sort(deque<ClassNode*>& q){
    for (int i = 0; i < q.size(); i++){
        for (int j = 1; j < q.size()-i; j++){
            if(q.at(j-1)->getName() > q.at(j)->getName()){
                ClassNode* temp = q.at(j-1);
                q.at(j-1) = q.at(j);
                q.at(j) = temp;
            }
        }
    }
}


/*
 * \brief Prints a course progression DAG topologically sorted
 * \note Most imortant part of lab
 */
void printTopSort(vector<ClassNode*> verticies){
    deque<ClassNode*> q = deque<ClassNode*>();
    unsigned int counter = 0;
    ClassNode* v = NULL;
    ClassNode* w = NULL;

    for(int i = 0; i < verticies.size(); i++){
        v = verticies.at(i);
        if(v->getNumInEdges() == 0){
            q.push_front(v);
        }
    }
        while(!q.empty()){
            sort(q);

            v = q.front();
            q.pop_front();
            counter++;
            cout << v->getName() + " ";
            vector<ClassNode*> edges = v->getOutEdges();
            for (int j = 0; j < edges.size(); j++){
                w = edges.at(j);
                w->decNumInEdges();
                if(w->getNumInEdges() == 0){
                    q.push_front(w);
                }
            }
        }
    
    cout << endl;
    if(counter != verticies.size()){
        cout << "ERROR: Cycle Detected" << endl;
    }
}

/**
 * \breif   Deletes dynamocally allocated memory
 */
void cleanUp(vector<ClassNode*> verticies){
    for (int i = 0; i < verticies.size(); i++){
        delete verticies.at(i);
    }
}


/**
 * \breif   Reads in the file and creates the graph
 */
void readInput(int argc, char** argv, vector<ClassNode*>& verticies){
    // verify the correct number of parameters
    if (argc != 2) {
        cout << "Must supply the input file name as the one and only parameter" << endl;
        exit(1);
    }

    // attempt to open the supplied file
    ifstream file(argv[1], ifstream::binary);
    // report any problems opening the file and then exit
    if (!file.is_open()) {
        cout << "Unable to open file '" << argv[1] << "'." << endl;
        exit(2);
    }

    string s1, s2;
    unordered_set<string> classesInGraph;
    while(true){
        ClassNode* fromVertex = NULL;
        ClassNode* toVertex = NULL;
        // read in two space delimited strings
        file >> s1; // From
        file >> s2; // To

        if(s1 == "0" && s2 == "0"){
            break;
        }

        unordered_set<string>::const_iterator found = classesInGraph.find(s1);
        
        if(found == classesInGraph.end()){
            //Not In Set. Make the Vertex
            classesInGraph.insert(s1);
            fromVertex = new ClassNode(s1);
            verticies.push_back(fromVertex);
        }
        else{
            //find in verticies
            for(int i = 0; i < verticies.size(); i++){
                if (verticies.at(i)->getName() == s1){
                    fromVertex = verticies.at(i);
                }
            }
        }

        found = classesInGraph.find(s2);
        if(found == classesInGraph.end()){
            //Not In Set. Make the Vertex
            classesInGraph.insert(s2);
            toVertex = new ClassNode(s2);
            verticies.push_back(toVertex);
        }
        else{
            //find in verticies
            for(int i = 0; i < verticies.size(); i++){
                if (verticies.at(i)->getName() == s2){
                    toVertex = verticies.at(i);
                }
            }
        }

        fromVertex->incNumOutEdges();
        fromVertex->addOutEdge(toVertex);
        toVertex->incNumInEdges();
    }

    // close the file before exiting
    file.close();
}

/**
 * \brief   Runs the Pre-lab program
 * \remark requires 1 comand line argument of a .txt file
 */
int main(int argc, char** argv) {
    vector<ClassNode*> verticies = vector<ClassNode*>();
    readInput(argc, argv, verticies);
    bubbleSort(verticies);
    //printGraph(verticies); //For debugging
    printTopSort(verticies);
    cleanUp(verticies);
    return 0;
}