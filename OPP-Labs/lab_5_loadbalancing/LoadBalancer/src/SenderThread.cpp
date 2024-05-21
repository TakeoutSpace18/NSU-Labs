#include "SenderThread.h"

#include <iostream>
#include <vector>

#include "BlockingQueue.h"
#include "Task.h"
#include "TasksRequest.h"

constexpr int MIN_TASKS_IN_QUEUE_TO_SHARE = 5;
constexpr int MAX_TASKS_TO_SHARE = 40;

SenderThread::SenderThread(const MPI::Comm& communicator, std::shared_ptr<BlockingQueue<Task>> taskQueue)
    : mComm(communicator), mTaskQueue(std::move(taskQueue)), mSender(&SenderThread::EntryPoint, this)
{
    mProcCount = mComm.Get_size();
    mProcRank = mComm.Get_rank();
}

void SenderThread::EntryPoint()
{
    while (!mStopped) {
        std::optional<TasksRequest> request = ReceiveTasksRequest(300);

        if (!request.has_value()) {
            continue;
        }

        std::vector<Task> tasksToSend;
        for (int i = 0; i < std::min(request->wantedCount, MAX_TASKS_TO_SHARE); ++i) {
            std::optional<Task> task = mTaskQueue->TryPop();
            if (!task.has_value() || mTaskQueue->Size() < MIN_TASKS_IN_QUEUE_TO_SHARE) {
                break;
            }
            tasksToSend.push_back(std::move(task.value()));
            mTotalTasksSent++;
        }

        mComm.Send(tasksToSend.data(), tasksToSend.size() * sizeof(Task), MPI::CHAR, request->sourceProcRank, TASKS_TAG);
        if (tasksToSend.empty()) {
            // std::cout << "Proc " + std::to_string(mProcRank) + "; sent " + std::to_string(tasksToSend.size()) +  " tasks\n";

        }
    }
}

std::optional<TasksRequest> SenderThread::ReceiveTasksRequest(int timeoutMs)
{
    using namespace std::chrono;

    TasksRequest received = {};
    MPI::Request recvRequest = mComm.Irecv(&received, sizeof(received), MPI::CHAR, MPI::ANY_SOURCE, TASK_REQUEST_TAG);

    auto startTime = steady_clock::now();

    // Loop until we have received, or taken too long
    while (!recvRequest.Test() && duration_cast<milliseconds>(steady_clock::now() - startTime).count() < timeoutMs) {
        std::this_thread::sleep_for(milliseconds(30));
    }

    if (!recvRequest.Test()) {
        // We must have timed out
        recvRequest.Cancel();
        recvRequest.Free();
        return std::nullopt;
    }

    return received;
}
