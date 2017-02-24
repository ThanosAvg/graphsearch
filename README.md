ACM SIGMOD 2016 Programming Contest Project Implementation

Description Link: http://dsg.uwaterloo.ca/sigmod16contest/task.html

Authors:Thanos Avgetidis (https://github.com/ThanosAvg) Georgios Anastasiou (https://github.com/gsiou)

The task is to answer shortest path queries on a changing graph(dynamic graphs), as quickly as possible.
The graph is unweighted and undirected.

(Dynamic Graphs)
Α connected component index was built to determine whether a possible connection between nodes exists.
An update index array was used to connect different connected components after insertions.

(Static Graphs)
The SIGMOD contest task was extended and searching in static graphs was also implemented with the use
of a grail index(*) over the hypergraph created by the strongly connected components(SCC's) of the graph.
An iterative version of tarjan's algorithm was used for finding the SCC's.

The program is multithreaded using POSIX threads. 

(*)Link: http://www.vldb.org/pvldb/vldb2010/pvldb_vol3/R24.pdf
