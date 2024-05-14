#ifndef RECIEVERTHREAD_H
#define RECIEVERTHREAD_H

#include <condition_variable>
#include <functional>
#include <utility>
#include <mpi.h>

class WorkerThread;
class Task;

template <class T>
class BlockingQueue;

class RecieverThread {
public:
    RecieverThread(const MPI::Comm& communicator,
        std::shared_ptr<BlockingQueue<Task>> taskQueue);

    void RequestTasks();
    void SetNoMoreTasksCallback(std::function<void()> callback) { mNoMoreTasksCallback = std::move(callback); }

private:
    void EntryPoint();

private:
    std::function<void()> mNoMoreTasksCallback;

    const MPI::Comm& mComm;
    int mProcRank;
    int mProcCount;

    std::shared_ptr<BlockingQueue<Task>> mTaskQueue;

    std::condition_variable mTasksRequest;
    std::mutex mMutex;
    std::thread mReciever;
};



#endif //RECIEVERTHREAD_H
