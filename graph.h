#ifndef GRAPH_H
#define GRAPH_H

#define MAX_NODES 20
#define INF 999999

// 校园地点
typedef struct {
    int id;
    char name[30];
} Place;

// 邻接矩阵图
typedef struct {
    int adj[MAX_NODES][MAX_NODES]; // 边权（步行分钟）
    int node_count;
    Place places[MAX_NODES];
} Graph;

void InitGraph(Graph *g);
int AddPlace(Graph *g, const char *name);
void AddEdge(Graph *g, int u, int v, int weight);
void Dijkstra(Graph *g, int start, int dist[], int prev[]);
void PrintPath(Graph *g, int prev[], int start, int end);
int FindPlaceByName(Graph *g, const char *name);
void PrintMap(Graph *g);

#endif