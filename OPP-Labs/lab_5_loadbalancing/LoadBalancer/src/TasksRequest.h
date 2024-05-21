#ifndef TASKSREQUEST_H
#define TASKSREQUEST_H

constexpr int TASK_REQUEST_TAG = 777;

struct TasksRequest
{
    int sourceProcRank;
    int wantedCount;
};

#endif //TASKSREQUEST_H
