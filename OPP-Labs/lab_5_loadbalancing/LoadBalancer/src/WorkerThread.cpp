#include "WorkerThread.h"

#include <iostream>
#include <utility>

#include "ReceiverThread.h"
#include "BlockingQueue.h"
#include "Task.h"

WorkerThread::WorkerThread(
    std::shared_ptr<BlockingQueue<Task>> taskQueue,
    std::shared_ptr<ReceiverThread> recieverThread)
    : mRecieverThread(std::move(recieverThread)), mTaskQueue(std::move(taskQueue)),
      mWorker(&WorkerThread::EntryPoint, this), mStopped(false)
{
}


void WorkerThread::EntryPoint()
{
    // Don't finish until task queue is empty,
    // even is stop method had been called.
    while (!mStopped || !mTaskQueue->Empty()) {
        mRecieverThread->RequestTasks();

        std::optional<Task> task = mTaskQueue->WaitAndPop();
        if (task.has_value()) {
            task->Execute();
            mTotalTasksExecuted++;
        }
    }
}
