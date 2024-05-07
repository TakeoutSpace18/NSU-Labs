#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#include <condition_variable>
#include <queue>


template<class T>
class BlockingQueue {
public:
    BlockingQueue() = default;

    void Push(T&& value)
    {
        std::lock_guard lock(mMutex);
        mQueue.push(std::forward<T>(value));
        mCondVar.notify_one();
    }

    T Pop()
    {
        std::unique_lock lock(mMutex);
        mCondVar.wait(lock, !mQueue.empty());
        return mQueue.pop();
    }

    [[nodiscard]] std::size_t Size() const
    {
        return mQueue.size();
    }

    [[nodiscard]] bool Empty() const
    {
        return mQueue.empty();
    }

private:
    std::queue<T> mQueue;
    std::condition_variable mCondVar;
    std::mutex mMutex;
};


#endif //BLOCKINGQUEUE_H
