#include "graph.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static Graph g;
static TaskHeap heap;
static int next_task_id = 1;

// 已完成任务记录
static Task done_tasks[MAX_TASKS];
static int done_count = 0;

// 构建校园地图
static void BuildCampusMap() {
    InitGraph(&g);
    AddPlace(&g, "1号宿舍");
    AddPlace(&g, "2号宿舍");
    AddPlace(&g, "3号宿舍");
    AddPlace(&g, "第一食堂");
    AddPlace(&g, "第二食堂");
    AddPlace(&g, "快递中心");
    AddPlace(&g, "图书馆");
    AddPlace(&g, "教学楼A");
    AddPlace(&g, "教学楼B");
    AddPlace(&g, "校医院");
    AddPlace(&g, "北门");
    AddPlace(&g, "南门");

    // 添加道路（步行分钟）
    AddEdge(&g, 0, 3, 4);   // 1号宿舍 - 一食堂
    AddEdge(&g, 0, 5, 6);   // 1号宿舍 - 快递中心
    AddEdge(&g, 1, 3, 3);
    AddEdge(&g, 1, 4, 5);
    AddEdge(&g, 1, 5, 7);
    AddEdge(&g, 2, 4, 4);
    AddEdge(&g, 2, 8, 6);
    AddEdge(&g, 3, 6, 5);   // 一食堂 - 图书馆
    AddEdge(&g, 3, 7, 4);
    AddEdge(&g, 4, 8, 5);
    AddEdge(&g, 4, 6, 7);
    AddEdge(&g, 5, 7, 8);
    AddEdge(&g, 5, 10, 10); // 快递中心 - 北门
    AddEdge(&g, 6, 7, 3);
    AddEdge(&g, 6, 9, 6);   // 图书馆 - 校医院
    AddEdge(&g, 7, 8, 4);
    AddEdge(&g, 8, 11, 9);  // 教学楼B - 南门
    AddEdge(&g, 9, 10, 7);
    AddEdge(&g, 10, 11, 12);
}

static void PrintMenu() {
    printf("\n======= 校园跑腿任务调度系统 =======\n");
    printf("1. 查看校园地图\n");
    printf("2. 发布跑腿任务\n");
    printf("3. 查看待接任务（优先级排序）\n");
    printf("4. 接单并规划路线\n");
    printf("5. 查看已完成任务与收入\n");
    printf("6. 随机生成一批任务\n");
    printf("0. 退出\n");
    printf("====================================\n");
    printf("请选择: ");
}

static void PublishTask() {
    Task t;
    t.id = next_task_id++;
    printf("发布人姓名: "); scanf("%s", t.publisher);
    printf("任务类型(0取快递/1带饭/2买药/3其他): ");
    int type; scanf("%d", &type);
    t.type = (TaskType)type;

    PrintMap(&g);
    printf("起点编号: "); scanf("%d", &t.from_node);
    printf("终点编号: "); scanf("%d", &t.to_node);
    printf("赏金(元): "); scanf("%d", &t.reward);
    printf("紧急度(1~5): "); scanf("%d", &t.urgency);
    printf("期望送达(分钟): "); scanf("%d", &t.deadline);

    t.is_done = 0;
    t.accepted = 0;
    HeapPush(&heap, t);
    printf("任务发布成功！\n");
}

static void RandomTasks() {
    int n;
    printf("生成任务数量: "); scanf("%d", &n);
    srand((unsigned)time(NULL));
    const char *names[] = {"小明", "小红", "小李", "小张", "小王", "小刘"};
    for (int i = 0; i < n; i++) {
        Task t;
        t.id = next_task_id++;
        strcpy(t.publisher, names[rand() % 6]);
        t.type = (TaskType)(rand() % 4);
        t.from_node = rand() % g.node_count;
        do {
            t.to_node = rand() % g.node_count;
        } while (t.to_node == t.from_node);
        t.reward = 2 + rand() % 10;
        t.urgency = 1 + rand() % 5;
        t.deadline = 10 + rand() % 40;
        t.is_done = 0;
        t.accepted = 0;
        HeapPush(&heap, t);
    }
    printf("已生成 %d 个任务！\n", n);
}

// 接单：从堆顶取出优先级最高的任务
static void AcceptTask() {
    if (HeapEmpty(&heap)) {
        printf("暂无任务可接！\n");
        return;
    }
    Task t;
    HeapPop(&heap, &t);

    printf("\n------ 接到任务 #%d ------\n", t.id);
    printf("发布人: %s\n", t.publisher);
    printf("类型: %s\n", TaskTypeName(t.type));
    printf("赏金: %d 元  紧急度: %d\n", t.reward, t.urgency);

    // 计算最短路径
    int dist[MAX_NODES], prev[MAX_NODES];
    Dijkstra(&g, t.from_node, dist, prev);

    printf("起点: %s\n", g.places[t.from_node].name);
    printf("终点: %s\n", g.places[t.to_node].name);
    if (dist[t.to_node] == INF) {
        printf("? 两点之间不可达，任务取消。\n");
        return;
    }
    printf("最短距离: %d 分钟\n", dist[t.to_node]);
    printf("推荐路线: ");
    PrintPath(&g, prev, t.from_node, t.to_node);

    if (dist[t.to_node] <= t.deadline)
        printf("? 可在期望时间内送达（%d <= %d 分钟）\n", dist[t.to_node], t.deadline);
    else
        printf("? 可能超时（需 %d 分钟 > %d 分钟）\n", dist[t.to_node], t.deadline);

    t.accepted = 1;
    t.is_done = 1;
    done_tasks[done_count++] = t;
    printf("任务已完成，收入 +%d 元！\n", t.reward);
}

static void PrintDone() {
    if (done_count == 0) {
        printf("还没有完成任何任务。\n");
        return;
    }
    int total = 0;
    printf("\n========== 已完成任务 ==========\n");
    printf("%-4s %-10s %-8s %-6s\n", "ID", "发布人", "类型", "赏金");
    for (int i = 0; i < done_count; i++) {
        Task *t = &done_tasks[i];
        printf("%-4d %-10s %-8s %-6d\n",
               t->id, t->publisher, TaskTypeName(t->type), t->reward);
        total += t->reward;
    }
    printf("--------------------------------\n");
    printf("总收入: %d 元\n", total);
    printf("================================\n");
}

int main() {
    BuildCampusMap();
    InitHeap(&heap);

    int choice;
    while (1) {
        PrintMenu();
        if (scanf("%d", &choice) != 1) break;
        switch (choice) {
            case 1: PrintMap(&g); break;
            case 2: PublishTask(); break;
            case 3: PrintTasks(&heap); break;
            case 4: AcceptTask(); break;
            case 5: PrintDone(); break;
            case 6: RandomTasks(); break;
            case 0:
                printf("再见！\n");
                return 0;
            default:
                printf("无效选择！\n");
        }
    }
    return 0;
}
