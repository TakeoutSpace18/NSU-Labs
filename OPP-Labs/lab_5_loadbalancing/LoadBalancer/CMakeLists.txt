find_package(MPI)
if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    message("MPI_INCLUDE_PATH :${MPI_INCLUDE_PATH}")
endif (MPI_FOUND)

add_executable(LoadBalancer src/main.cpp
        src/BlockingQueue.h
        src/WorkerThread.cpp
        src/WorkerThread.h
        src/Task.h
        src/ReceiverThread.cpp
        src/ReceiverThread.h
        src/SenderThread.cpp
        src/SenderThread.h
        src/TasksRequest.h)

target_include_directories(LoadBalancer PRIVATE src)
target_compile_options(LoadBalancer PRIVATE -Wall -Wextra -Wpedantic -O3)

target_link_libraries(LoadBalancer MPI::MPI_CXX)

add_executable(BlockingQueueTest src/BlockingQueue.h test/BlockingQueueTest.cpp)
target_include_directories(BlockingQueueTest PRIVATE src test)
