#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <list>
#include <random>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "apis_c.h"

#define N 100  // 假设图中有100个节点
#define chiplet 2

void create_shared_memory() {
    // 创建共享内存对象
    int shm_fd = shm_open("/visited", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // 配置共享内存大小
    if (ftruncate(shm_fd, N * sizeof(bool)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // 映射共享内存
    bool* visited = static_cast<bool*>(mmap(nullptr, N * sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (visited == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 初始化共享内存
    for (int i = 0; i < N; ++i) {
        visited[i] = false;
    }

    // 解除映射和关闭共享内存
    munmap(visited, N * sizeof(bool));
    close(shm_fd);
}

int main(int argc, char **argv) {
    create_shared_memory();
    int idX = atoi(argv[1]);
    int idY = atoi(argv[2]);
    // 使用随机数生成器创建图
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 10); // 边的长度在1到10之间
    std::uniform_int_distribution<> node_dist(0, 99); // 节点编号在0到99之间

    int num_edges = 200; // 总共添加200条边
    float *start = new float[200];
    float *end = new float[200];
    float *Length = new float[200];
    for (int i = 0; i < num_edges; ++i) {
        int v = node_dist(gen);
        int w = node_dist(gen);
        if (v != w) { // 避免自环
            int length = dist(gen);
            // g->addEdge(v, w, length);
            start[i] = v;
            end[i] = w;
            Length[i] = length;
        }
    }
    for (int i = 0; i < 100; i++) {
        // g->addEdge(i, i, 0);
        start[i] = i;
        end[i] = i;
        Length[i] = 0;
    }
    InterChiplet::sendMessage(0, 1, idX, idY, start, sizeof(float)*num_edges);
    InterChiplet::sendMessage(0, 1, idX, idY, end, sizeof(float)*num_edges);
    InterChiplet::sendMessage(0, 1, idX, idY, Length, sizeof(float)*num_edges);
    int start_vertex = 0;
    InterChiplet::sendMessage(0, 1, idX, idY, &start_vertex, sizeof(int));
    
    bool **END = new bool* [chiplet-1];
    for(int i=0;i<chiplet-1;i++){
        END[i] = new bool;
    }
    for(int i=0;i<chiplet-1;i++){
        InterChiplet::receiveMessage(idX, idY, 0, 1,  END[i], sizeof(bool));
    }
    // bfs(g, start_vertex, visited);
    delete[] start;
    delete[] end;
    delete[] Length;

    return 0;
}