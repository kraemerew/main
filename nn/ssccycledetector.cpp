#include "ssccycledetector.hpp"


using namespace std;


SScCycleDetector::SScCycleDetector(int V)
{
    this->V = V;
    adj = new list<int>[V];
}

void SScCycleDetector::addEdge(int v, int w)
{
    adj[v].push_back(w);
}

bool SScCycleDetector::isCyclicUtil(int v, bool visited[], bool *recStack)
{
    if (visited[v] == false)
    {
        visited[v] = true;
        recStack[v] = true;
        list<int>::iterator i;
        for(i = adj[v].begin(); i != adj[v].end(); ++i)
        {
            if (!visited[*i] && isCyclicUtil(*i, visited, recStack))
                return true;
            else if (recStack[*i])
                return true;
        }
    }
    recStack[v] = false;
    return false;
}

bool SScCycleDetector::isCyclic()
{
    bool visited[V], recStack[V];
    for (int i = 0; i < V; i++)
    {
        visited[i] = false;
        recStack[i] = false;
    }
    for (int i = 0; i < V; i++)
        if (isCyclicUtil(i, visited, recStack))
            return true;
    return false;
}
