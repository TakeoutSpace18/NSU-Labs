find_package(MPI)
if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    message("MPI_INCLUDE_PATH :${MPI_INCLUDE_PATH}")
endif (MPI_FOUND)

find_package(fmt)

add_executable(LoadBalancer src/main.cpp
        src/BlockingQueue.h
        src/WorkerThread.cpp
        src/WorkerThread.h
        src/Task.h
        src/RecieverThread.cpp
        src/RecieverThread.h
        src/SenderThread.cpp
        src/SenderThread.h
        src/TasksRequest.h
        test/BlockingQueueTest.cpp)

target_include_directories(LoadBalancer PRIVATE src)
target_compile_options(LoadBalancer PRIVATE -Wall -Wextra -Wpedantic -O0 -g)

target_link_libraries(LoadBalancer MPI::MPI_CXX fmt::fmt)

add_executable(BlockingQueueTest src/BlockingQueue.h test/BlockingQueueTest.cpp)
target_include_directories(BlockingQueueTest PRIVATE src test)
