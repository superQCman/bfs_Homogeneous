#include <iostream>
#include <vector>
#include <queue>
#include "graph.h"
#include "kernel.h"
#include "apis_c.h"


void bfs(Node* h_graph_nodes, int* h_graph_edges, bool* h_graph_mask, bool* h_graph_visited, int* h_cost, int no_of_nodes, int edge_list_size) {
    bool h_over;
    do {
        h_over = false;
        bfsKernel(h_graph_nodes, h_graph_edges, h_graph_mask, h_graph_visited, h_cost, &h_over, no_of_nodes);
    } while (h_over);
}

int main(int argc, char **argv) {
    int idX = atoi(argv[1]);
    int idY = atoi(argv[2]);
    int no_of_nodes = 6;
    int edge_list_size = 8;


    int *h_graph_nodes_start = new int[no_of_nodes];


    int *h_graph_nodes_no_of_edges = new int[no_of_nodes];



    int *h_graph_edges = new int[edge_list_size];


    bool *h_graph_mask = new bool[no_of_nodes];


    bool *h_graph_visited = new bool[no_of_nodes];

    int *h_cost = new int[no_of_nodes];
    InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_nodes_start, no_of_nodes * sizeof(int));
    InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_nodes_no_of_edges, no_of_nodes * sizeof(int));
    InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_edges, edge_list_size * sizeof(int));
    InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_mask, no_of_nodes * sizeof(bool));
    InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_visited, no_of_nodes * sizeof(bool));
    InterChiplet::receiveMessage(idX, idY, 0, 0, h_cost, no_of_nodes * sizeof(int));

    Node *h_graph_nodes = new Node[no_of_nodes];

    for (int i = 0; i < no_of_nodes; ++i) {
        h_graph_nodes[i].starting = h_graph_nodes_start[i];
        h_graph_nodes[i].no_of_edges = h_graph_nodes_no_of_edges[i];
    }
    bfs(h_graph_nodes, h_graph_edges, h_graph_mask, h_graph_visited, h_cost, no_of_nodes, edge_list_size);
    for (int i = 0; i < no_of_nodes; i++) {
        std::cout << "Node " << i << " cost: " << h_cost[i] << std::endl;
    }
    InterChiplet::sendMessage(0, 0, idX, idY,  h_cost, no_of_nodes * sizeof(int));

    delete[] h_graph_nodes;
    delete[] h_graph_nodes_start;
    delete[] h_graph_nodes_no_of_edges;
    delete[] h_graph_edges;
    delete[] h_graph_mask;
    delete[] h_graph_visited;
    delete[] h_cost;

    return 0;
}