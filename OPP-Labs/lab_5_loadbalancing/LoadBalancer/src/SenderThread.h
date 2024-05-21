#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <memory>
#include <mpi.h>
#include <optional>
#include <thread>

#include "TasksRequest.h"

class Task;

template <class T>
class BlockingQueue;

class SenderThread {
public:
    SenderThread(const MPI::Comm& communicator, std::shared_ptr<BlockingQueue<Task>> taskQueue);

    void Stop() { mStopped = true; }
    void Join() { mSender.join(); }

    int GetTotalTasksSent() { return mTotalTasksSent; }

private:
    void EntryPoint();
    std::optional<TasksRequest> ReceiveTasksRequest(int timeoutMs);

private:
    const MPI::Comm& mComm;
    int mProcRank;
    int mProcCount;

    std::shared_ptr<BlockingQueue<Task>> mTaskQueue;

    std::thread mSender;
    bool mStopped = false;

    int mTotalTasksSent = 0;
};



#endif //SENDERTHREAD_H
