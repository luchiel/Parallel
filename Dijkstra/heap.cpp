#include "heap.hpp"

using namespace std;

Heap::Heap(int start) {
    heap.push_back(make_pair(-1, -1));
    heap.push_back(make_pair(0, start));
}

void Heap::sift_up(int v) {
    while(v != 1 && heap[v] < heap[v/2]) {
        swap(heap[v], heap[v/2]);
        v = v / 2;
    }
}

void Heap::sift_down(int v) {
    int s = v;
    if(2 * v <= heap.size() - 1) {
        if(2 * v + 1 <= heap.size() - 1 && heap[2 * v + 1] < heap[2 * v])
            s = 2 * v + 1;
        else
            s = 2 * v;
    }
    if(heap[s] < heap[v]) {
        swap(heap[s], heap[v]);
        sift_down(s);
    }
}

void Heap::add_vertex(pair<int, int> v) {
    heap.push_back(v);
    sift_up(heap.size() - 1);
}

void Heap::delete_vertex(int v) {
    swap(heap[v], heap[heap.size() - 1]);
    heap.pop_back();
    if(heap.size() > 1)
        sift_down(v);
}

int Heap::top() {
    return heap[1].second;
}
