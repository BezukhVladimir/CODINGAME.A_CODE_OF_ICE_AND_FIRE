// https://bezukh.wixsite.com/blog/post/simple-pathfinding-algorithm-dijkstra

#include <iomanip>
#include <climits>
#include <iostream>

using namespace std;

const unsigned START = 0;
const unsigned VERTICES = 6;
const unsigned ADJACENCY_MATRIX[VERTICES][VERTICES] = {
    {0, 321, 733, 0, 469, 0},
    {242, 0, 0, 602, 0, 0},
    {467, 0, 0, 412, 0, 0},
    {0, 842, 356, 0, 0, 213},
    {0, 0, 0, 1107, 0, 895},
    {0, 0, 0, 0, 0, 0}};
    
void simplePathfindingAlgoritm(const unsigned ADJACENCY_MATRIX[VERTICES][VERTICES], const unsigned &START)
{
    bool visited[VERTICES];
    unsigned distances[VERTICES];
    unsigned minimalWeight, minimalIndex;
    
    for (unsigned i = 0; i < VERTICES; ++i)
    {
        visited[i] = false;
        distances[i] = INT_MAX;
    }
    
    distances[START] = 0;
    
    do
    {
        minimalIndex = INT_MAX;
        minimalWeight = INT_MAX;
        
        for (unsigned i = 0; i < VERTICES; ++i)
        {
            if (!visited[i] && distances[i] < minimalWeight)
            { 
                minimalIndex = i;
                minimalWeight = distances[i];
            }
        }
            
        if (minimalIndex != INT_MAX)
        {
            for (unsigned i = 0; i < VERTICES; ++i)
            {
                if (ADJACENCY_MATRIX[minimalIndex][i])
                {
                    unsigned temp = minimalWeight + ADJACENCY_MATRIX[minimalIndex][i];
                    
                    if (temp < distances[i])
                        distances[i] = temp;
                }
            }
        
            visited[minimalIndex] = true;
        }
    }
    while (minimalIndex < INT_MAX);
        
    for (unsigned i = 0; i < VERTICES; ++i)
    {
        if (distances[i] != INT_MAX)
        {
            cout << "Вес: " << START << " ~> " << i << " = " << setw(6) << left << distances[i] << "\t";
            
            unsigned end = i;
            unsigned weight = distances[end];
            string way = to_string(end) + " >~ ";
            
            while (end != START)
            {
                for (unsigned j = 0; j < VERTICES; ++j)
                {
                    if (ADJACENCY_MATRIX[j][end])
                    { 
                        int temp = weight - ADJACENCY_MATRIX[j][end]; 
                        
                        if (temp == distances[j]) 
                        {   
                            end = j;              
                            weight = temp; 
                            way += to_string(j) + " >~ ";
                        }
                    }
                }
            }
          
            cout << "Путь: ";
            
            for (int j = way.length() - 5; j >= 0; --j) 
                cout << way[j];
            
            cout << endl;
        }
        else 
            cout << "Вес: " << START << " ~ " << i << " = " << "маршрут недоступен" << endl;
    }
    
}

int main() 
{ 
    simplePathfindingAlgoritm(ADJACENCY_MATRIX, START);
}