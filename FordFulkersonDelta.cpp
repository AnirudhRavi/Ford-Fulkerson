#include <iostream>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <fstream>
#include <queue>

using namespace std;

#define V 20
#define INPUT  "Graph1.txt"			//Input File
#define OUTPUT "FordFulkersonOutput_Delta.txt"	//Output File

//Function Definitions
bool path_finder(int residual[V][V], int s, int t, int parent[]);   //Used for path finding
void mincut_finder(int residual[V][V], int s, bool visited2[]);     //Used for min cut
void max_flow(int graph[V][V], int s, int t);                        //Ford Fulkerson implementation
void print_mincut(int graph[V][V], bool visited2[], int s, int t);  //Print edges of the min cut
void update_deltares(int deltares[V][V], int residual[V][V], int delta);
			    
int main()
{
    freopen(OUTPUT, "w", stdout);
    std::ifstream ifptr(INPUT, std::ifstream::in);

    int graph[V][V];
    for(int i = 0; i < V; i++)
	for( int j = 0; j < V; j++)
		graph[i][j] = 0;

    int s, t, v1, v2, c;
    if ( ifptr.is_open() )
    {
	ifptr >> s; ifptr >> t;
    
	while( !ifptr.eof() )
	{
		ifptr >> v1; ifptr >> v2; ifptr >> c;
		graph[v1][v2] = c;
	}
    }
    
    struct timeval tstart, tend;
    gettimeofday (&tstart, NULL);

    max_flow(graph, s, t); 

    gettimeofday (&tend, NULL);

    cout << "\n\n\t \t \t TIME TAKEN\n";
    cout << "\t \t \t ____ _____\n \n"; 
    std::cout << "Max flow with Min Cut but with Delta Scaling took " << ((tend.tv_sec - tstart.tv_sec)*1000000L+tend.tv_usec)-tstart.tv_usec << " microseconds\n";       
    return 0;
}

//Returns max flow from s to t in given graph
void max_flow(int graph[V][V], int s, int t)
{
    int u, v, it = 0, mC = INT_MIN, delta = 1;
    
    //Finding maximum capacity coming out of source
    for( int i = 0; i < V; i++ )
    	mC = max(mC, graph[s][i]);

    //Getting the value of Delta
    while( delta < mC )
    	delta <<= 1;
    if( delta > mC )
	delta >>= 1;
    
    cout << "\t \t \t ALGORITHM\n";
    cout << "\t \t \t _________\n \n"; 
    
    int residual[V][V]; // Residual graph where residual[i][j] indicates residual capacity of edge from i to j
    int deltares[V][V];

    //Fill residual graph with intial capacities in directed graph
    for (u = 0; u < V; u++)
        for (v = 0; v < V; v++)
             residual[u][v] = graph[u][v];

    //Fill delta residual graph intiall as residual graph
    for (u = 0; u < V; u++)
        for (v = 0; v < V; v++)
             deltares[u][v] = residual[u][v];
 
    int path[V];  //Array used to fill augment path in residual graph
 
    int max_flow = 0;  //Intially no flow
    
    update_deltares(deltares, residual, delta);
    
    //Augment flow as long as there is a path from s to t
    while(delta >= 1)
    {
	cout << endl << "With Delta = " << delta << ":";
    	while (path_finder(deltares, s, t, path))
    	{
		cout << endl << "Iteration #" << ++it << ":" << endl << endl;
        	int path_flow = INT_MAX;
		cout << "Path = ";
        	for (v = t; v != s; v = path[v])
        	{
            		u = path[v];
            		//Bottleneck for augment path
            		path_flow = min(path_flow, residual[u][v]);
	    		if(u != s)
	    			cout << v << " <-(" << residual[u][v] << ")- ";
	    		else if( v != s)
				cout << s << endl;
        	}
 
        	//Update residual capacities
        	for (v = t; v != s; v = path[v])
        	{
	    		u = path[v];
            		residual[u][v] -= path_flow;
            		residual[v][u] += path_flow;
        	}
	 
        	// Adding bottleneck to overall flow
        	max_flow += path_flow;
		cout << "Bottleneck of path, b = " << path_flow << endl;
		cout << "New flow, f = " << max_flow << endl;
		update_deltares(deltares, residual, delta);
	}
        delta = delta/2;
	update_deltares(deltares, residual, delta);
    }
    
    //Find vertices reachable from s after flow is maximum
    bool visited2[V];
    memset(visited2, false, sizeof(visited2));
    mincut_finder(residual, s, visited2);
    
    print_mincut(graph, visited2, s, t);

    cout << "\n\n\t \t \t MAX FLOW\n";
    cout << "\t \t \t ___ ____\n \n";
    cout << "Max-flow value = " << max_flow << endl;
    cout << "Final Residual graph: (Adjacency Matrix representation)" << endl;
    for (u = 0; u < V; u++)
    {
	cout << endl;
        for (v = 0; v < V; v++)
             cout << residual[u][v] << "\t";
    }
}

//Returns true if path exists from s to t in residual graph. Stores path in path[]
bool path_finder(int residual[V][V], int s, int t, int path[])
{
    bool visited[V];        //To mark vertices visited using BFS
    memset(visited, 0, sizeof(visited));
 
    queue <int> q;          //Queue for maintaining all nodes to be visited
    q.push(s);              //Add s to queue
    visited[s] = true;      //Mark s as visited
    path[s] = -1;
 
    //Loop for BFS
    while ( !q.empty() )
    {
        int u = q.front();
        q.pop();
 
        for (int v = 0; v < V; v++)
        {
            if (visited[v] == false && residual[u][v] > 0)
            {
                q.push(v);
                path[v] = u;
                visited[v] = true;
            }
        }
    }
    //If t has been visited then path exists. Return true
    return (visited[t] == true);
}

//DFS to find all vertices reachable from s
void mincut_finder(int residual[V][V], int s, bool visited2[])
{
    visited2[s] = true;
    for (int i = 0; i < V; i++)
       if (residual[s][i] && !visited2[i])
           mincut_finder(residual, i, visited2);
}

//Print edged from A (vertices reacchable from s) to B (vertices not reachable from s)
void print_mincut(int graph[V][V], bool visited2[], int s, int t)
{
     int mincut_size = 0, mincut_cap = 0;
     cout << "\n\n\t \t \t MINCUT\n";
     cout << "\t \t \t ______\n \n"; 
     cout << "Edges from S (set of vertices reachable from source) to T (set of vertices not reachable from source) are:" << endl; 
     for (int i = 0; i < V; i++)
      for (int j = 0; j < V; j++)
         if (visited2[i] && !visited2[j] && graph[i][j])
	 {    
	      mincut_size++; mincut_cap += graph[i][j];
              cout << i << " - " << j << endl;
	 }
     cout << "Min-cut capacity = " << mincut_cap << endl;
     cout << "Min-cut:" << endl << "Set S = {" << s;
     for (int i = 0; i < V; i++)
    	if ( visited2[i] && i != s)
		cout << ", " << i;
     
     cout << "}" << endl << "Set T = {" << t;
     for (int i = 0; i < V; i++)
    	if ( !visited2[i] && i != t)
		cout << ", " << i;
     cout << "}" << endl;
}

void update_deltares(int deltares[V][V], int residual[V][V], int delta)
{
     for (int i = 0; i < V; i++)
      for (int j = 0; j < V; j++)
	if( residual[i][j] >= delta)
	   deltares[i][j] = residual[i][j];
	else	
	   deltares[i][j] = 0;
}
