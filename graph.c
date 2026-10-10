#include "graph.h"
#include <stdio.h>
#include <string.h>

void InitGraph(Graph *g) {
    g->node_count = 0;
    for (int i = 0; i < MAX_NODES; i++)
        for (int j = 0; j < MAX_NODES; j++)
            g->adj[i][j] = (i == j) ? 0 : INF;
}

int AddPlace(Graph *g, const char *name) {
    if (g->node_count >= MAX_NODES) return -1;
    int id = g->node_count++;
    g->places[id].id = id;
    strncpy(g->places[id].name, name, 29);
    g->places[id].name[29] = '\0';
    return id;
}

void AddEdge(Graph *g, int u, int v, int weight) {
    if (u < 0 || v < 0 || u >= g->node_count || v >= g->node_count) return;
    g->adj[u][v] = weight;
    g->adj[v][u] = weight; // 无向图
}

// 标准 Dijkstra，O(n^2)
void Dijkstra(Graph *g, int start, int dist[], int prev[]) {
    int visited[MAX_NODES] = {0};
    for (int i = 0; i < g->node_count; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[start] = 0;

    for (int i = 0; i < g->node_count; i++) {
        // 找未访问中 dist 最小的
        int u = -1, min = INF;
        for (int j = 0; j < g->node_count; j++) {
            if (!visited[j] && dist[j] < min) {
                min = dist[j];
                u = j;
            }
        }
        if (u == -1) break;
        visited[u] = 1;

        // 松弛
        for (int v = 0; v < g->node_count; v++) {
            if (!visited[v] && g->adj[u][v] != INF) {
                if (dist[u] + g->adj[u][v] < dist[v]) {
                    dist[v] = dist[u] + g->adj[u][v];
                    prev[v] = u;
                }
            }
        }
    }
}

// 递归打印路径
static void PrintPathRec(Graph *g, int prev[], int start, int cur) {
    if (cur == -1) return;
    if (cur == start) {
        printf("%s", g->places[start].name);
        return;
    }
    PrintPathRec(g, prev, start, prev[cur]);
    printf(" -> %s", g->places[cur].name);
}

void PrintPath(Graph *g, int prev[], int start, int end) {
    if (start == end) {
        printf("%s\n", g->places[start].name);
        return;
    }
    PrintPathRec(g, prev, start, end);
    printf("\n");
}

int FindPlaceByName(Graph *g, const char *name) {
    for (int i = 0; i < g->node_count; i++) {
        if (strcmp(g->places[i].name, name) == 0) return i;
    }
    return -1;
}

void PrintMap(Graph *g) {
    printf("\n========== 校园地图 ==========\n");
    for (int i = 0; i < g->node_count; i++) {
        printf("[%d] %s\n", i, g->places[i].name);
    }
    printf("---------- 道路 ----------\n");
    for (int i = 0; i < g->node_count; i++) {
        for (int j = i + 1; j < g->node_count; j++) {
            if (g->adj[i][j] != INF) {
                printf("%s <--> %s : %d 分钟\n",
                       g->places[i].name, g->places[j].name, g->adj[i][j]);
            }
        }
    }
    printf("==============================\n");
}