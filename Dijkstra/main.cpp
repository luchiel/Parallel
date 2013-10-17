#include <pthread.h>
#include <cstdio>
#include <vector>
#include <utility>
#include <algorithm>

#include "heap.hpp"

#define TEST_FILE "input.txt"

using namespace std;

const int infinity = 1000 * 1000 * 1000;

vector<vector<pair<int, int> > > edges;
vector<pair<int, int> > connected;
int vertex_n, edge_n, start;
int connected_n;
vector<int> dist;
vector<int> bkwd;

void read_data() {
	freopen(TEST_FILE, "r", stdin);
	scanf("%d%d%d", &vertex_n, &edge_n, &start);

	edges.resize(vertex_n);
	for(int i = 0; i < edge_n; ++i) {
		int sv, ev, w;
		scanf("%d%d%d", &sv, &ev, &w);
		edges[sv].push_back(make_pair(ev, w));
		edges[ev].push_back(make_pair(sv, w));
	}

	scanf("%d", &connected_n);
	for(int i = 0; i < connected_n; ++i) {
		int sc, ec;
		scanf("%d%d", &sc, &ec);
		connected.push_back(make_pair(sc, ec));
	}
}

void init() {
	dist.clear();
	dist.resize(vertex_n, infinity);
	bkwd.resize(vertex_n);
}

pair<vector<int>*, int> dijkstra(int start, int end) {
	dist[start] = 0;
	Heap heap(start);
	while(!heap.empty()) {
		int e = heap.top();
		heap.delete_vertex(1);

		if(e == end) {
			break;
		}

		for(auto i = edges[e].begin(); i != edges[e].end(); ++i) {
			if(i->first < start) {
				continue;
			}

			int d = dist[e] + i->second;

			if(d < dist[i->first]) {
				dist[i->first] = d;
				bkwd[i->first] = e;
				heap.add_vertex(make_pair(d, i->first));
			}
		}
	}

	vector<int> *path = new vector<int>;
	for(int v = bkwd[end]; v != start; v = bkwd[v]) {
		path->push_back(v);
	}
	path->push_back(start);
	reverse(path->begin(), path->end());
	return make_pair(path, dist[end]);
}

void *dijkstra_thread(void *args) {
	pair<int, int> se = *static_cast<pair<int, int>*>(args);
	int start = se.first;
	int end = se.second;

	pair<vector<int>*, int> *result = new pair<vector<int>*, int>;
	*result = dijkstra(start, end);

	for(int i = start; i <= end; ++i) {
		if(i == start || i == end) {
			for(auto j = edges[i].begin(); j != edges[i].end(); ++j) {
				if(j->first < start || j->first > end) {
					int v = j->first;
					int w = j->second;

					edges[i].clear();
					edges[i].push_back(make_pair(v, w));

					break;
				}
			}
		}
		else {
			edges[i].clear();
		}
	}
	edges[start].push_back(make_pair(end, result->second));
	edges[end].push_back(make_pair(start, result->second));

	return result->first;
}

int main() {
	read_data();
	init();

	vector<pthread_t> threads(connected_n);
	for(int i = 0; i < connected_n; ++i) {
		pthread_create(&threads[i], NULL, dijkstra_thread, &connected[i]);
	}

	vector<vector<int> > results_connected(connected_n);
	for(int i = 0; i < connected_n; ++i) {
		void *result = new vector<int>;
		pthread_join(threads[i], &result);
		results_connected[i] = *static_cast<vector<int>*>(result);
	}

	init();
	pair<vector<int>*, int> result = dijkstra(start, vertex_n - 1);

	printf("Distance: %d\n", result.second);

	for(auto i = result.first->begin(); i != result.first->end(); ++i) {
		printf("%d ",  *i);
		for(auto j = results_connected.begin(); j != results_connected.end(); ++j) {
			if(*i == (*j)[0]) {
				for(int k = 1; k < j->size(); ++k) {
					printf("%d ", (*j)[k]);
				}
				break;
			}
		}
	}
	printf("%d\n", vertex_n - 1);
	return 0;
}
