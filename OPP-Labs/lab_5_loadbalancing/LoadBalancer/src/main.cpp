#include <fstream>
#include <iostream>
#include <memory>
#include <mpi.h>
#include <sstream>
#include <unistd.h>

#include "BlockingQueue.h"
#include "ReceiverThread.h"
#include "SenderThread.h"
#include "Task.h"
#include "WorkerThread.h"

constexpr int ITER_TASKS_COUNT = 2000;
constexpr int ITER_COUNT = 33;

void GenerateTasks(std::shared_ptr<BlockingQueue<Task>> queue, int procRank, int procCount, int iterCount)
{
    for (int i = 0; i < ITER_TASKS_COUNT; ++i) {
        queue->Emplace(1 + std::abs(procRank - (iterCount % procCount)));
    }
}

int main(int argc, char **argv)
{
    int provided = MPI::Init_thread(argc, argv, MPI::THREAD_MULTIPLE);
    if (provided != MPI::THREAD_MULTIPLE) {
        throw std::runtime_error("MPI implementation is not thread-compliant!");
    }

    int procRank = MPI::COMM_WORLD.Get_rank();
    int procCount = MPI::COMM_WORLD.Get_size();

    std::cout << "Proc: " + std::to_string(procRank) + "; pid: " + std::to_string(getpid()) + "\n";

    auto taskQueue = std::make_shared<BlockingQueue<Task>>();

    auto receiverThread = std::make_shared<ReceiverThread>(MPI::COMM_WORLD, taskQueue);
    auto senderThread = std::make_shared<SenderThread>(MPI::COMM_WORLD, taskQueue);

    std::ofstream plot("plot_proc_" + std::to_string(procRank) + ".dat");

    for (int curIter = 0; curIter < ITER_COUNT; ++curIter) {
        MPI::COMM_WORLD.Barrier();
        if (procRank == 0) {
            std::cout << "Began " + std::to_string(curIter) +  " iteration...\n";
        }

        GenerateTasks(taskQueue, procRank, procCount, curIter);

        double beginTime = MPI::Wtime();

        WorkerThread workerThread(taskQueue, receiverThread);
        receiverThread->SetNoMoreTasksCallback([&](){
            workerThread.Stop();
            taskQueue->InterruptWaiting();
        });
        workerThread.Join();

        double endTime = MPI::Wtime();
        double timeElapsed = endTime - beginTime;

        plot << curIter << " " << timeElapsed << "\n";
        std::stringstream ss;
        ss << "Proc " << procRank << ": Iter " << curIter << "; time elapsed: " << timeElapsed << "\n";
        ss << "\tTotal tasks received: " << receiverThread->GetTotalTasksReceived() << "\n";
        ss << "\tTotal tasks sent: " << senderThread->GetTotalTasksSent() << "\n";
        ss << "\tTotal tasks executed: " << workerThread.GetTotalTasksExecuted() << "\n";
        std::cout << ss.str() << std::endl;
    }

    MPI::COMM_WORLD.Barrier();

    receiverThread->Stop();
    senderThread->Stop();

    receiverThread->Join();
    senderThread->Join();

    plot.close();

    MPI::COMM_WORLD.Barrier();

    MPI::Finalize();
    return EXIT_SUCCESS;
}