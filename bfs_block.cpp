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

class Graph {
public:
    std::unordered_map<int, std::list<std::pair<int, int>>> adj; // 邻接表，存储边和边的长度

    void addEdge(int v, int w, int length) {
        adj[v].emplace_back(w, length); // 添加一个从v到w的边，长度为length
    }
};

void bfs(Graph const& graph, int start_vertex, bool* visited) {
    std::queue<int> queue;
    std::mutex queue_mutex;
    std::condition_variable cv;

    visited[start_vertex] = true;
    queue.push(start_vertex);

    auto worker = [&]() {
        while (true) {
            int current_vertex = 0;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                if (!queue.empty()) {
                    current_vertex = queue.front();
                    queue.pop();
                }
                else {
                    break;
                }
            }
            std::cout<<"------------------------------------------------------------------------------------------"<<std::endl;
            std::cout << "Visited " << current_vertex << std::endl;
            std::cout<<"------------------------------------------------------------------------------------------"<<std::endl;
            for (const auto& neighbor : graph.adj.at(current_vertex)) {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (!visited[neighbor.first]) {
                    visited[neighbor.first] = true;
                    queue.push(neighbor.first);
                    cv.notify_all();  // 通知等待的线程
                }
            }
        }
     };

    // 创建两个线程
    std::thread t1(worker);
    std::thread t2(worker);
    // 等待两个线程结束
    t1.join();
    t2.join();
}

int main(int argc, char **argv){
    Graph g;
    int idX = atoi(argv[1]);
    int idY = atoi(argv[2]);
    int num_edges = 200; // 总共添加200条边
    float *start = new float[200];
    float *end = new float[200];
    float *Length = new float[200];

    InterChiplet::receiveMessage(idX, idY, 0, 0, start, 200 * sizeof(float));
    for (int i = 0; i < num_edges; ++i) {
        std::cout << "start[" << i << "] = " << start[i] << std::endl;
    }
    InterChiplet::receiveMessage(idX, idY, 0, 0, end, 200 * sizeof(float));
    InterChiplet::receiveMessage(idX, idY, 0, 0, Length, 200 * sizeof(float));
    int start_vertex=0;
    InterChiplet::receiveMessage(idX, idY, 0, 0, &start_vertex, sizeof(int));

    // 打开共享内存对象
    int shm_fd = shm_open("/visited", O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // 映射共享内存
    bool* visited = static_cast<bool*>(mmap(nullptr, 100 * sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (visited == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    for (int i = 0; i < num_edges; ++i) {
        int v = start[i];
        int w = end[i];
        if (v != w) { // 避免自环
            int length = Length[i];
            g.addEdge(v, w, length);
        }
    }
    for (int i = 0; i < 100; i++) {
        g.addEdge(i, i, 0);
    }
    bfs(g, start_vertex, visited);

    // 解除映射和关闭共享内存
    munmap(visited, 100 * sizeof(bool));
    close(shm_fd);

    delete[] start;
    delete[] end;
    delete[] Length;
    bool END = true;
    InterChiplet::sendMessage( 0, 0,idX, idY, &END, sizeof(bool));
    return 0;
}