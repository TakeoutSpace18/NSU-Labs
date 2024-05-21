#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include <memory>
#include <thread>

class Task;

template <class T>
class BlockingQueue;

class ReceiverThread;

class WorkerThread {
public:
    explicit WorkerThread(std::shared_ptr<BlockingQueue<Task>> taskQueue,
        std::shared_ptr<ReceiverThread> recieverThread);

    void Stop() { mStopped = true; }
    void Join() { mWorker.join(); }

    [[nodiscard]] int GetTotalTasksExecuted() const { return mTotalTasksExecuted; }

private:
    void EntryPoint();

    std::shared_ptr<ReceiverThread> mRecieverThread;
    std::shared_ptr<BlockingQueue<Task>> mTaskQueue;
    std::thread mWorker;
    bool mStopped;

    int mTotalTasksExecuted = 0;
};



#endif //WORKERTHREAD_H
