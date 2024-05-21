#include "ReceiverThread.h"

#include <numeric>
#include <random>
#include <utility>

#include "BlockingQueue.h"
#include "Task.h"
#include "TasksRequest.h"
#include "WorkerThread.h"

constexpr int MIN_TASKS_IN_QUEUE_TO_REQUEST_NEW = 10;
constexpr int WANTED_TASKS_COUNT = 20000;

ReceiverThread::ReceiverThread(const MPI::Comm& communicator,
                               std::shared_ptr<BlockingQueue<Task>> taskQueue)
    : mComm(communicator), mTaskQueue(std::move(taskQueue)), mReciever(&ReceiverThread::EntryPoint, this) {
    mProcCount = mComm.Get_size();
    mProcRank = mComm.Get_rank();
}

void ReceiverThread::RequestTasks()
{
    mTasksRequested.store(true);
    mWakeUp.notify_one();
}

void ReceiverThread::Stop()
{
    mStopped = true;
    mWakeUp.notify_one();
}

void ReceiverThread::EntryPoint()
{
    std::unique_lock lock(mMutex);
    while (!mStopped) {
        mWakeUp.wait_for(lock, std::chrono::milliseconds(300));
        if (!mTasksRequested.load()) {
            continue;
        }

        mTasksRequested.store(false);
        if (mTaskQueue->Size() > MIN_TASKS_IN_QUEUE_TO_REQUEST_NEW) {
            continue;
        }

        std::vector<int> procRequestOrder(mProcCount);
        std::iota(procRequestOrder.begin(), procRequestOrder.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(procRequestOrder.begin(), procRequestOrder.end(), g);

        TasksRequest request = {};
        request.sourceProcRank = mProcRank;
        request.wantedCount = WANTED_TASKS_COUNT;

        int receivedTasksCount = 0;
        for (const int proc : procRequestOrder) {
            if (proc == mProcRank) {
                // don't request tasks from yourself
                continue;
            }

            mComm.Send(&request, sizeof(request), MPI::CHAR, proc, TASK_REQUEST_TAG);

            std::vector<Task> receivedTasks = ReceiveTasks(proc, 300);

            for (Task& task : receivedTasks) {
                mTaskQueue->Push(std::move(task));
            }

            receivedTasksCount += receivedTasks.size();
            request.wantedCount -= receivedTasks.size();
            if (request.wantedCount <= 0) {
                break;
            }
        }

        mTotalTasksReceived += receivedTasksCount;

        if (receivedTasksCount == 0) {
            mNoMoreTasksCallback();
        }
    }
}

std::vector<Task> ReceiverThread::ReceiveTasks(int fromProc, int timeoutMs) const
{
    using namespace std::chrono;

    MPI::Status tasksRecvStatus;
    auto startTime = steady_clock::now();

    // Loop until we have received, or taken too long
    while (!mComm.Iprobe(fromProc, TASKS_TAG, tasksRecvStatus)
        && duration_cast<milliseconds>(steady_clock::now() - startTime).count() < timeoutMs) {
        std::this_thread::sleep_for(milliseconds(30));
    }

    if (!mComm.Iprobe(fromProc, TASKS_TAG, tasksRecvStatus)) {
        return {};
    }

    std::vector<Task> receivedTasks(tasksRecvStatus.Get_count(MPI::CHAR) / sizeof(Task));
    mComm.Recv(receivedTasks.data(), tasksRecvStatus.Get_count(MPI::CHAR), MPI::CHAR, fromProc, TASKS_TAG);

    return receivedTasks;
}
