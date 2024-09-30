#include <iostream>
#include <vector>
#include <queue>
#include "graph.h"
#include "kernel.h"
#include "apis_c.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
# include <semaphore.h>
# include <thread>


void bfs(int tid ,Node* h_graph_nodes, int* h_graph_edges, bool* h_graph_mask, bool* h_graph_visited, int* h_cost, int no_of_nodes, int edge_list_size) {
    bool h_over;
    do {
        h_over = false;
        bfsKernel(tid,h_graph_nodes, h_graph_edges, h_graph_mask, h_graph_visited, h_cost, &h_over, no_of_nodes);
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
    int *tid = new int;
    InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_nodes_start, no_of_nodes * sizeof(int));
    InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_nodes_no_of_edges, no_of_nodes * sizeof(int));
    InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_edges, edge_list_size * sizeof(int));
    InterChiplet::receiveMessage(idX, idY, 0, 0, tid, sizeof(int));
    // InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_mask, no_of_nodes * sizeof(bool));
    // InterChiplet::receiveMessage(idX, idY, 0, 0, h_graph_visited, no_of_nodes * sizeof(bool));
    // InterChiplet::receiveMessage(idX, idY, 0, 0, h_cost, no_of_nodes * sizeof(int));
    int shm_fd;
    // 打开并读取共享内存
    shm_fd = shm_open("/h_graph_mask", O_RDWR, 0666);
    h_graph_mask = (bool *)mmap(0, no_of_nodes * sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shm_fd = shm_open("/h_graph_visited", O_RDWR, 0666);
    h_graph_visited = (bool *)mmap(0, no_of_nodes * sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shm_fd = shm_open("/h_cost", O_RDWR, 0666);
    h_cost = (int *)mmap(0, no_of_nodes * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    Node *h_graph_nodes = new Node[no_of_nodes];

    for (int i = 0; i < no_of_nodes; ++i) {
        h_graph_nodes[i].starting = h_graph_nodes_start[i];
        h_graph_nodes[i].no_of_edges = h_graph_nodes_no_of_edges[i];
    }
    bfs(*tid,h_graph_nodes, h_graph_edges, h_graph_mask, h_graph_visited, h_cost, no_of_nodes, edge_list_size);
   
    for (int i = 0; i < no_of_nodes; i++) {
        std::cout << "Node " << i << " cost: " << h_cost[i] << std::endl;
    }
    // InterChiplet::sendMessage(0, 0, idX, idY,  h_cost, no_of_nodes * sizeof(int));

        // 解除映射和关闭共享内存
    munmap(h_graph_mask, no_of_nodes * sizeof(bool));
    munmap(h_graph_visited, no_of_nodes * sizeof(bool));
    munmap(h_cost, no_of_nodes * sizeof(int));

    delete[] h_graph_nodes;
    delete[] h_graph_nodes_start;
    delete[] h_graph_nodes_no_of_edges;
    delete[] h_graph_edges;
    delete[] h_graph_mask;
    delete[] h_graph_visited;
    delete[] h_cost;

    return 0;
}