#ifndef RECEIVERTHREAD_H
#define RECEIVERTHREAD_H

#include <condition_variable>
#include <functional>
#include <thread>
#include <atomic>
#include <utility>

#include <mpi.h>

class WorkerThread;
class Task;

template <class T>
class BlockingQueue;

class ReceiverThread {
public:
    ReceiverThread(const MPI::Comm& communicator,
        std::shared_ptr<BlockingQueue<Task>> taskQueue);

    void RequestTasks();
    void SetNoMoreTasksCallback(std::function<void()> callback) { mNoMoreTasksCallback = std::move(callback); }
    void Stop();
    void Join() { mReciever.join(); }

    int GetTotalTasksReceived() { return mTotalTasksReceived; }

private:
    void EntryPoint();
    std::vector<Task> ReceiveTasks(int fromProc, int timeoutMs) const;

private:
    std::function<void()> mNoMoreTasksCallback;

    const MPI::Comm& mComm;
    int mProcRank;
    int mProcCount;

    std::shared_ptr<BlockingQueue<Task>> mTaskQueue;

    std::condition_variable mWakeUp;
    std::atomic<bool> mTasksRequested = false;
    std::mutex mMutex;
    std::thread mReciever;
    bool mStopped = false;

    int mTotalTasksReceived = 0;
};



#endif //RECEIVERTHREAD_H
