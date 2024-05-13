#include "RecieverThread.h"

#include <utility>

#include "WorkerThread.h"

RecieverThread::RecieverThread(const MPI::Comm& communicator,
                               std::shared_ptr<BlockingQueue<Task>> taskQueue)
    : mComm(communicator), mTaskQueue(std::move(taskQueue)), mReciever(&RecieverThread::EntryPoint, this)
{
    mWorldSize = mComm.Get_size();
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

        // TODO: request tasks from other processes

        bool tasksAvailable = false;

        if (!tasksAvailable) {
            mNoMoreTasksCallback();
        }
    }
}
