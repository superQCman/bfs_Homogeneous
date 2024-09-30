#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <fcntl.h>           // O_CREAT, O_RDWR
#include <sys/mman.h>        // mmap, shm_open
#include <unistd.h>          // ftruncate
#include <cstring>
#include "apis_c.h"
#include "graph.h"


void send(int idX, int idY, int *h_graph_nodes_start, int *h_graph_nodes_no_of_edges, int *h_graph_edges, int no_of_nodes, int edge_list_size, int *tid) {
    InterChiplet::sendMessage(0, *tid+1, idX, idY, h_graph_nodes_start, no_of_nodes * sizeof(int));
    InterChiplet::sendMessage(0, *tid+1, idX, idY, h_graph_nodes_no_of_edges, no_of_nodes * sizeof(int));
    InterChiplet::sendMessage(0, *tid+1, idX, idY, h_graph_edges, edge_list_size * sizeof(int));
    InterChiplet::sendMessage(0, *tid+1, idX, idY, tid, sizeof(int));
}
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

    int shm_fd;
    bool *h_graph_mask;
    bool *h_graph_visited;
    int *h_cost;

    // Create shared memory object
    shm_fd = shm_open("/h_graph_mask", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, no_of_nodes * sizeof(bool));
    h_graph_mask = (bool *)mmap(0, no_of_nodes * sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shm_fd = shm_open("/h_graph_visited", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, no_of_nodes * sizeof(bool));
    h_graph_visited = (bool *)mmap(0, no_of_nodes * sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shm_fd = shm_open("/h_cost", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, no_of_nodes * sizeof(int));
    h_cost = (int *)mmap(0, no_of_nodes * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Initialize shared memory
    h_graph_mask[0] = true;
    for (int i = 1; i < no_of_nodes; i++) {
        h_graph_mask[i] = false;
    }
    for (int i = 0; i < no_of_nodes; i++) {
        h_graph_visited[i] = false;
        h_cost[i] = -1;
    }
    h_cost[0] = 0;
    int *tid = new int [no_of_nodes];
    for(int i;i<no_of_nodes;i++){
        tid[i] = i;
        send(idX, idY, h_graph_nodes_start, h_graph_nodes_no_of_edges, h_graph_edges, no_of_nodes, edge_list_size, &tid[i]);
    }
    // InterChiplet::sendMessage(0, 1, idX, idY, h_graph_nodes_start, no_of_nodes * sizeof(int));
    // InterChiplet::sendMessage(0, 1, idX, idY, h_graph_nodes_no_of_edges, no_of_nodes * sizeof(int));
    // InterChiplet::sendMessage(0, 1, idX, idY, h_graph_edges, edge_list_size * sizeof(int));
    // InterChiplet::sendMessage(0, 1, idX, idY, h_graph_mask, no_of_nodes * sizeof(bool));
    // InterChiplet::sendMessage(0, 1, idX, idY, h_graph_visited, no_of_nodes * sizeof(bool));
    // InterChiplet::sendMessage(0, 1, idX, idY, h_cost, no_of_nodes * sizeof(int));

    // InterChiplet::receiveMessage(idX, idY, 0, 1, h_cost, no_of_nodes * sizeof(int));

    for (int i = 0; i < no_of_nodes; i++) {
        std::cout << "Node " << i << " cost: " << h_cost[i] << std::endl;
    }
    // 解除映射和关闭共享内存
    munmap(h_graph_mask, no_of_nodes * sizeof(bool));
    munmap(h_graph_visited, no_of_nodes * sizeof(bool));
    munmap(h_cost, no_of_nodes * sizeof(int));
    shm_unlink("/h_graph_mask");
    shm_unlink("/h_graph_visited");
    shm_unlink("/h_cost");
}