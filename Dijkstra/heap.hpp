#ifndef HEAP_HPP
#define HEAP_HPP

#include <vector>
#include <utility>

using namespace std;

class Heap {
    vector<pair<int, int> > heap;

    void sift_up(int v);
    void sift_down(int v);

public:
    void add_vertex(pair<int, int> v);
    void delete_vertex(int v);
    bool empty() { return heap.size() == 1; }
    int top();

    Heap(int start);
};

#endif
