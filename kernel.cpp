// kernel.cu
#include <iostream>
#include "graph.h"
#include "kernel.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>

void bfsKernel(int tid,Node* g_graph_nodes, int* g_graph_edges, bool* g_graph_mask, bool* g_graph_visited, int* g_cost, bool* g_over, int no_of_nodes) {
    // sem_wait(sem);
    if (g_graph_mask[tid]) {
        g_graph_mask[tid] = false;
        g_graph_visited[tid] = true;
        for (int i = g_graph_nodes[tid].starting; i < (g_graph_nodes[tid].no_of_edges + g_graph_nodes[tid].starting); ++i) {
            int id = g_graph_edges[i];
            if (!g_graph_visited[id]) {
                g_cost[id] = g_cost[tid] + 1;
                g_graph_mask[id] = true;
                *g_over = true;
            }
        }
    }
    // sem_post(sem);
}