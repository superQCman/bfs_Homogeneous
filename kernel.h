#ifndef KERNEL_H
#define KERNEL_H
#include <iostream>
#include "graph.h"

void bfsKernel(int tid,Node* g_graph_nodes, int* g_graph_edges, bool* g_graph_mask, bool* g_graph_visited, int* g_cost, bool* g_over, int no_of_nodes);


#endif // GRAPH_H