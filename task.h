#ifndef TASK_H
#define TASK_H

#define MAX_TASKS 100

// 任务类型
typedef enum {
    TASK_EXPRESS = 0,  // 取快递
    TASK_FOOD,         // 带饭
    TASK_MEDICINE,     // 买药
    TASK_OTHER         // 其他
} TaskType;

// 任务
typedef struct {
    int id;
    char publisher[20];   // 发布人
    TaskType type;
    int from_node;        // 起点（一般是发布人所在地）
    int to_node;          // 终点（送达地）
    int reward;           // 赏金（元）
    int urgency;          // 紧急度 1~5
    int deadline;         // 期望送达时间（分钟）
    int is_done;          // 是否完成
    int accepted;         // 是否被接单
} Task;

// 最大堆：优先级 = urgency * 100 + reward
typedef struct {
    Task data[MAX_TASKS];
    int size;
} TaskHeap;

void InitHeap(TaskHeap *h);
int HeapEmpty(TaskHeap *h);
void HeapPush(TaskHeap *h, Task t);
int HeapPop(TaskHeap *h, Task *t);
void PrintTasks(TaskHeap *h);
const char *TaskTypeName(TaskType t);

#endif