#include "RecieverThread.h"

#include <iostream>
#include <numeric>
#include <random>
#include <ranges>
#include <utility>

#include "BlockingQueue.h"
#include "Task.h"
#include "TasksRequest.h"
#include "WorkerThread.h"

RecieverThread::RecieverThread(const MPI::Comm& communicator,
                               std::shared_ptr<BlockingQueue<Task>> taskQueue)
    : mComm(communicator), mTaskQueue(std::move(taskQueue)), mReciever(&RecieverThread::EntryPoint, this)
{
    mProcCount = mComm.Get_size();
    mProcRank = mComm.Get_rank();
}

void RecieverThread::RequestTasks()
{
    mTasksRequest.notify_one();
}

void RecieverThread::EntryPoint()
{
    std::unique_lock lock(mMutex);
    while (true) {
        mTasksRequest.wait(lock);

        std::vector<int> procRequestOrder(mProcCount);
        std::iota(procRequestOrder.begin(), procRequestOrder.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(procRequestOrder.begin(), procRequestOrder.end(), g);

        TasksRequest request = {
            .wantedCount = 5
        };

        int totalTasksReceived = 0;
        for (const int proc : procRequestOrder) {
            if (proc == mProcRank) {
                // don't request tasks from yourself
                continue;
            }

            mComm.Send(&request, sizeof(request), MPI::CHAR, proc, TASK_REQUEST_TAG);
            // std::cout << "Proc " + std::to_string(mProcRank) + "; sent request " << std::to_string(request.wantedCount) <<  " tasks\n";

            MPI::Status tasksRecvStatus;
            mComm.Probe(proc, TASKS_TAG, tasksRecvStatus);

            std::vector<Task> receivedTasks(tasksRecvStatus.Get_count(MPI::CHAR) / sizeof(Task));
            mComm.Recv(receivedTasks.data(), tasksRecvStatus.Get_count(MPI::CHAR), MPI::CHAR, proc, TASKS_TAG);
            // std::cout << "Proc " + std::to_string(mProcRank) + "; received " << std::to_string(receivedTasks.size()) <<  " tasks\n";

            for (Task& task : receivedTasks) {
                mTaskQueue->Push(std::move(task));
            }

            totalTasksReceived += receivedTasks.size();
            request.wantedCount -= receivedTasks.size();
            if (request.wantedCount <= 0) {
                break;
            }
        }

        if (totalTasksReceived == 0) {
            mNoMoreTasksCallback();
        }
    }
}
