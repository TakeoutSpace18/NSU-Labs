#include "SenderThread.h"

SenderThread::SenderThread(const MPI::Comm& communicator, std::shared_ptr<BlockingQueue<Task>> taskQueue)
    : mComm(communicator), mTaskQueue(std::move(taskQueue))
{
    mWorldSize = mComm.Get_size();
    mProcRank = mComm.Get_rank();
}
