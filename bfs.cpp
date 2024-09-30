#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include "apis_c.h"
#include "graph.h"

int main(int argc, char **argv){
    int idX = atoi(argv[1]);
    int idY = atoi(argv[2]);
    int no_of_nodes = 6;
    int edge_list_size = 8;

    int h_graph_nodes_start[6] = {
        0, 2, 3, 4, 5, 6
    };
    int h_graph_nodes_no_of_edges[6] = {
        2, 1, 1, 1, 1, 1
    };
    // Node h_graph_nodes[6] = {
    //     {0, 2}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}
    // };

    int h_graph_edges[8] = {
        1, 2, 3, 4, 5, 0, 0, 0
    };

    bool h_graph_mask[6] = {true, false, false, false, false, false};
    bool h_graph_visited[6] = {false, false, false, false, false, false};
    int h_cost[6] = {-1, -1, -1, -1, -1, -1};
    h_cost[0] = 0;

    InterChiplet::sendMessage(0, 1, idX, idY, h_graph_nodes_start, no_of_nodes * sizeof(int));
    InterChiplet::sendMessage(0, 1, idX, idY, h_graph_nodes_no_of_edges, no_of_nodes * sizeof(int));
    InterChiplet::sendMessage(0, 1, idX, idY, h_graph_edges, edge_list_size * sizeof(int));
    InterChiplet::sendMessage(0, 1, idX, idY, h_graph_mask, no_of_nodes * sizeof(bool));
    InterChiplet::sendMessage(0, 1, idX, idY, h_graph_visited, no_of_nodes * sizeof(bool));
    InterChiplet::sendMessage(0, 1, idX, idY, h_cost, no_of_nodes * sizeof(int));

    InterChiplet::receiveMessage(idX, idY, 0, 1, h_cost, no_of_nodes * sizeof(int));

    for (int i = 0; i < no_of_nodes; i++) {
        std::cout << "Node " << i << " cost: " << h_cost[i] << std::endl;
    }
}