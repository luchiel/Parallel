#include <pthread.h>
#include <cstdio>
#include <vector>
#include <utility>
#include <queue>

#include "heap.hpp"

using namespace std;

const int infinity = 1000 * 1000 * 1000;

vector<vector<pair<int, int> > > edges;
vector<pair<int, int> > connected;
int vertex_n, edge_n, start;
int connected_n;
vector<int> dist;
/*
Задан граф, состоящий из нескольких компонент связности.
известно, что компоненты связности связанны между собой.

Задача - найти путь из начальной в конечную точку алгоритмом Дейкстры, если заданы компоненты связности.

Задача состоит в запуске алгоритма Дейкстры на каждую компоненту связности и потом на граф,
вершинами которого являются компоненты связности. 
*/

void read_data_and_init() {
	freopen("input.txt", "r", stdin);
	scanf("%d%d%d", &vertex_n, &edge_n, &start);

	edges.resize(vertex_n);
	for(int i = 0; i < edge_n; ++i) {
		int sv, ev, w;
		scanf("%d%d%d", &sv, &ev, &w);
		edges[sv].push_back(make_pair(ev, w));
		edges[ev].push_back(make_pair(sv, w)); //non-oriented edges
	}

	scanf("%d", &connected_n);
	for(int i = 0; i < connected_n; ++i) {
		int sc, ec;
		scanf("%d%d", &sc, &ec);
		connected.push_back(make_pair(sc, ec));
	}

	dist.resize(vertex_n);
	for(int i = 0; i < vertex_n; ++i) {
		dist[i] = infinity;
	}
}

int dijkstra(int start, int end) {
	// visited?
	dist[start] = 0;
	Heap heap(start);
	while(!heap.empty()) {
		int e = heap.top();
		heap.delete_vertex(1);

		if(e == end) {
			break;
		}

        // if(vis[c])
        //     continue;
        // vis[c] = true;

		// if(dist[e] == infinity) {
		// 	break;
		// }

		for(auto i = edges[e].begin(); i != edges[e].end(); ++i) {
			if(i->first < start) {
				continue;
			}

			int d = dist[e] + i->second;

			// also if !vis[i->first] && ..
			if(d < dist[i->first]) {
				dist[i->first] = d;
				heap.add_vertex(make_pair(d, i->first));
			}
		}
	}
	return dist[end];
}

void *dijkstra_thread(void *args) {
	pair<int, int> se = *static_cast<pair<int, int>*>(args);
	int *d = new int;
	*d = dijkstra(se.first, se.second);
	return d;
}

int main() {
	read_data_and_init();

	vector<pthread_t> threads(connected_n);
	for(int i = 0; i < connected_n; ++i) {
		pthread_create(&threads[i], NULL, dijkstra_thread, &connected[i]);
	}

	vector<int> results_connected(connected_n);
	for(int i = 0; i < connected_n; ++i) {
		void *r = new int;
		pthread_join(threads[i], &r);
		results_connected[i] = *static_cast<int*>(r);
	}
	return 0;
}
