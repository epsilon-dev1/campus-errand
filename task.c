#include "task.h"
#include <stdio.h>

// 优先级分数：紧急度权重更高
static int Priority(Task t) {
    return t.urgency * 100 + t.reward;
}

void InitHeap(TaskHeap *h) {
    h->size = 0;
}

int HeapEmpty(TaskHeap *h) {
    return h->size == 0;
}

static void Swap(Task *a, Task *b) {
    Task t = *a; *a = *b; *b = t;
}

static void Up(TaskHeap *h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (Priority(h->data[i]) > Priority(h->data[p])) {
            Swap(&h->data[i], &h->data[p]);
            i = p;
        } else break;
    }
}

static void Down(TaskHeap *h, int i) {
    while (1) {
        int l = 2 * i + 1, r = 2 * i + 2, best = i;
        if (l < h->size && Priority(h->data[l]) > Priority(h->data[best])) best = l;
        if (r < h->size && Priority(h->data[r]) > Priority(h->data[best])) best = r;
        if (best == i) break;
        Swap(&h->data[i], &h->data[best]);
        i = best;
    }
}

void HeapPush(TaskHeap *h, Task t) {
    if (h->size >= MAX_TASKS) {
        printf("任务已满！\n");
        return;
    }
    h->data[h->size] = t;
    Up(h, h->size);
    h->size++;
}

int HeapPop(TaskHeap *h, Task *t) {
    if (h->size == 0) return 0;
    *t = h->data[0];
    h->data[0] = h->data[h->size - 1];
    h->size--;
    Down(h, 0);
    return 1;
}

const char *TaskTypeName(TaskType t) {
    switch (t) {
        case TASK_EXPRESS:  return "取快递";
        case TASK_FOOD:     return "带饭";
        case TASK_MEDICINE: return "买药";
        default:            return "其他";
    }
}

void PrintTasks(TaskHeap *h) {
    if (h->size == 0) {
        printf("暂无待接任务。\n");
        return;
    }
    printf("\n========== 待接任务（按优先级排序） ==========\n");
    printf("%-4s %-10s %-8s %-8s %-6s %-6s %-6s\n",
           "ID", "发布人", "类型", "起点", "赏金", "紧急", "状态");
    // 复制一份堆，避免破坏原堆
    TaskHeap tmp = *h;
    while (!HeapEmpty(&tmp)) {
        Task t;
        HeapPop(&tmp, &t);
        printf("%-4d %-10s %-8s %-8d %-6d %-6d %-6s\n",
               t.id, t.publisher, TaskTypeName(t.type),
               t.from_node, t.reward, t.urgency,
               t.accepted ? "已接单" : "待接单");
    }
    printf("==============================================\n");
}