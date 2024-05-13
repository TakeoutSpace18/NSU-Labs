#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include <memory>
#include <thread>

class Task;

template <class T>
class BlockingQueue;

class RecieverThread;

class WorkerThread {
public:
    explicit WorkerThread(std::shared_ptr<BlockingQueue<Task>> taskQueue,
        std::shared_ptr<RecieverThread> recieverThread);

    void Stop() { mStopped = true; }
    void Join() { mWorker.join(); }

private:
    void EntryPoint() const;

    std::shared_ptr<RecieverThread> mRecieverThread;
    std::shared_ptr<BlockingQueue<Task>> mTaskQueue;
    std::thread mWorker;
    bool mStopped;
};



#endif //WORKERTHREAD_H
