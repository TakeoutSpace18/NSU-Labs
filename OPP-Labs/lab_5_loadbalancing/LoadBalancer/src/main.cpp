#include <fstream>
#include <iostream>
#include <memory>
#include <mpi.h>
#include <sstream>

#include "BlockingQueue.h"
#include "RecieverThread.h"
#include "SenderThread.h"
#include "Task.h"
#include "WorkerThread.h"

constexpr int ITER_TASKS_COUNT = 10;
constexpr int ITER_COUNT = 30;

void GenerateTasks(std::shared_ptr<BlockingQueue<Task>> queue, int procRank, int procCount, int iterCount)
{
    for (int i = 0; i < ITER_TASKS_COUNT; ++i) {
        // queue->Emplace(std::abs(procRank - (iterCount % procCount)));
        queue->Emplace(100);
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

    std::shared_ptr<BlockingQueue<Task>> taskQueue = std::make_shared<BlockingQueue<Task>>();

    std::shared_ptr<RecieverThread> recieverThread = std::make_shared<RecieverThread>(MPI::COMM_WORLD, taskQueue);
    std::shared_ptr<SenderThread> senderThread = std::make_shared<SenderThread>(MPI::COMM_WORLD, taskQueue);

    std::ofstream plot("plot_proc_" + std::to_string(procRank) + ".dat");

    for (int curIter = 0; curIter < ITER_COUNT; ++curIter) {
        GenerateTasks(taskQueue, procRank, procCount, curIter);

        MPI::COMM_WORLD.Barrier();
        double beginTime = MPI::Wtime();
        WorkerThread workerThread(taskQueue, recieverThread); // TODO: remove thread, make simple Worker?
        recieverThread->SetNoMoreTasksCallback([&](){
            workerThread.Stop();
            taskQueue->InterruptWaiting();
        });
        workerThread.Join();
        double endTime = MPI::Wtime();

        double timeElapsed = endTime - beginTime;

        plot << curIter << " " << timeElapsed << "\n";
        std::stringstream ss;
        ss << "Proc " << procRank << "; Iter " << curIter << "; time elapsed: " << timeElapsed << "\n";
        std::cout << ss.str();
    }


    MPI::Finalize();
    return EXIT_SUCCESS;
}