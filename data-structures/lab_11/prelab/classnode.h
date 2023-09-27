/*
Robert Owens (rao7utn)
11/16/2020
classnode.h
*/


#ifndef CLASSNODE_H
#define CLASSNODE_H

#include <vector>

using namespace std;

class ClassNode {
public:
    /**
     * \brief   Defualt Constructor for ClassNode
     * \remark  Not used
     */
    ClassNode();

    /**
     * \brief   Overloaded Constructor for ClassNode
     * \param   n is a const string passed by value. Becomes the name of the ClassNode
     */
    ClassNode(const string& n);
    /**
     * \brief   Defualt Destructor
     * \note    Empty, this class does not dynamically allocate memory
     */
    ~ClassNode();

    /**
     * \brief   Prints out all useful data about the ClassNode object
     * Data such as, class name. Number of in edges, Number of out egdes
     * As well as all outedges class name
     * \test    very useful for testing
     */
    void print();

    /**
     * \brief   Prints out all the out edges ClassNode names
     * \remark  only called in print()
     */
    void printOutEdges();

    /**
     * \brief   name getter
     * \return  a string that is the same of the ClassNode
     */
    string getName();

    /**
     * \brief   numInEdges getter
     * \return  an unsigned int with the number of incoming edges
     */
    unsigned int getNumInEdges();

    /**
     * \brief   numOutEdges getter
     * \return  an unsigned int with the number of outgoing edges
     */
    unsigned int getNumOutEdges();

    /**
     * \brief   returns a vector of ClassNodes which are outgoing edges
     * \return  std vector of the out going edges (ClassNode*)
     */
    vector<ClassNode*> getOutEdges();

    /**
     * \brief   name setter
     * \param   string n which will become the new class name
     */
    void setName(string n);

    /**
     * \brief   preforms numInEdges++
     */
    void incNumInEdges();

    /**
     * \brief   preforms numInEdges--
     */
    void decNumInEdges();

    /**
     * \brief   setter for numInEdges
     * \param   unsigned int i to become the new numInEdges value
     */
    void setNumInEdges(unsigned int i);

    /**
     * \brief   preforms numOutEdges++
     */
    void incNumOutEdges();

    /**
     * \brief   preforms numOutEdges--
     */
    void decNumOutEdges();

    /**
     * \brief   setter for numOutEdges
     * \param   unsigned int i becomes the new numOutEdges value
     */
    void setNumOutEdges(unsigned int i);

    /**
     * \brief   Adds a new out edge to the vector outEdges
     * \remark  the outEdges vector will always be sorted lexographically by ClassNode name
     */
    void addOutEdge(ClassNode* n);

private:
    string name;
    unsigned int numInEdges; //from
    unsigned int numOutEdges; //to
    vector<ClassNode*> outEdges; //outward edges

    void bubbleSort();
};

#endif