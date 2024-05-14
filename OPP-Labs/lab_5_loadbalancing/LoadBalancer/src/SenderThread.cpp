#include "SenderThread.h"

#include <iostream>
#include <vector>

#include "BlockingQueue.h"
#include "Task.h"
#include "TasksRequest.h"

SenderThread::SenderThread(const MPI::Comm& communicator, std::shared_ptr<BlockingQueue<Task>> taskQueue)
    : mComm(communicator), mTaskQueue(std::move(taskQueue)), mSender(&SenderThread::EntryPoint, this)
{
    mProcCount = mComm.Get_size();
    mProcRank = mComm.Get_rank();
}

void SenderThread::EntryPoint()
{
    while (true) {
        TasksRequest request;
        MPI::Status requestStatus;
        mComm.Recv(&request, sizeof(request), MPI::CHAR, MPI::ANY_SOURCE, TASK_REQUEST_TAG, requestStatus);
        // std::cout << "Proc " + std::to_string(mProcRank) + "; recieved request " << std::to_string(request.wantedCount) <<  " tasks\n";

        std::vector<Task> tasksToSend;
        for (int i = 0; i < request.wantedCount; ++i) {
            std::optional<Task> task = mTaskQueue->TryPop();
            if (!task.has_value()) {
                break;
            }
            tasksToSend.push_back(std::move(task.value()));
        }

        mComm.Send(tasksToSend.data(), tasksToSend.size() * sizeof(Task), MPI::CHAR, requestStatus.Get_source(), TASKS_TAG);
        // std::cout << "Proc " + std::to_string(mProcRank) + "; sent " << std::to_string(tasksToSend.size()) <<  " tasks\n";
    }
}
