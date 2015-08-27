**Program**: Programs to implement Ford Fulkerson algorithm to calculate min-cut and max-flow of a given flow network. Delta scaling has also been implemented to reduce worst case time complexity. 

**Author**: Anirudh Ravi

**About**:

These programs are written in C++. The algorithm implemented make use of the Ford-Fulkerson algorithm and chooses the augment path with the smallest number of edges by using breadth first search. When BFS is used along with Ford Fulkerson it is known as the Edmond-Karp algorithm. The running time of this algorithm is polynomial in E (number of edges) and V (number of vertices of the graph). The running time is O(V* E^2); since each augment path is found in O(E) time. Finding the min-cut after max-flow has been found for a graph, has also been implemented. Depth first search is used to find all vertices reachable from source s in the residual graph. This forms one partition of the min-cut while the rest of the vertices form the other partition. Delta scaling has also been used to reduce worst case time complexity to O(E^2 * log2C) where C is maximum flow from source s.

**Executing**:

g++ ProgramName -o ExecutableName

./ExecutableName

[Ensure Graph1.txt is present in the same directory]
