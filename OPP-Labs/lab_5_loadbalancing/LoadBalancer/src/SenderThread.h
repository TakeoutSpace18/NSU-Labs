#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <memory>
#include <mpi.h>
#include <thread>

class Task;

template <class T>
class BlockingQueue;

class SenderThread {
public:
    SenderThread(const MPI::Comm& communicator, std::shared_ptr<BlockingQueue<Task>> taskQueue);

private:
    void EntryPoint();

private:
    const MPI::Comm& mComm;
    int mProcRank;
    int mProcCount;

    std::shared_ptr<BlockingQueue<Task>> mTaskQueue;

    std::thread mSender;
};



#endif //SENDERTHREAD_H
