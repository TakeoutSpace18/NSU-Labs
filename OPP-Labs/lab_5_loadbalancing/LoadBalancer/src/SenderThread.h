#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <memory>
#include <mpi.h>

class Task;

template <class T>
class BlockingQueue;

class SenderThread {
public:
    SenderThread(const MPI::Comm& communicator, std::shared_ptr<BlockingQueue<Task>> taskQueue);

private:
    const MPI::Comm& mComm;
    int mProcRank;
    int mWorldSize;

    std::shared_ptr<BlockingQueue<Task>> mTaskQueue;
};



#endif //SENDERTHREAD_H
