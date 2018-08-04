#ifndef SSCCYCLEDETECTOR_HPP
#define SSCCYCLEDETECTOR_HPP


#include <iostream>
#include <list>
#include <climits>
#include <cstdlib>

using namespace std;

class SScCycleDetector
{
    private:
        int V;
        std::list<int>* adj;
        bool isCyclicUtil(int v, bool visited[], bool *rs);

    public:

        SScCycleDetector(int V);

        void addEdge(int v, int w);

        bool isCyclic();

};
#endif // SSCCYCLEDETECTOR_HPP
