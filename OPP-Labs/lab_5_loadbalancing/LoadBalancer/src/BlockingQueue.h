#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#include <condition_variable>
#include <optional>
#include <queue>


template<class T>
class BlockingQueue {
public:
    BlockingQueue() = default;

    void Push(T&& value)
    {
        std::lock_guard lock(mMutex);
        mQueue.push(std::forward<T>(value));
        mCondVar.notify_all();
    }

    template<class... Args>
    void Emplace(Args&&... args)
    {
        std::lock_guard lock(mMutex);
        mQueue.emplace(std::forward<Args...>(args...));
        mCondVar.notify_all();
    }

    std::optional<T> TryPop()
    {
        std::unique_lock lock(mMutex);

        if (mQueue.empty()) {
            return std::nullopt;
        }

        T ret = mQueue.front();
        mQueue.pop();
        return ret;
    }

    std::optional<T> TryFront()
    {
        std::unique_lock lock(mMutex);

        if (mQueue.empty()) {
            return std::nullopt;
        }

        return mQueue.front();
    }

    std::optional<T> WaitAndPop()
    {
        std::unique_lock lock(mMutex);
        while (mQueue.empty() && !mInterrupted) {
            mCondVar.wait(lock);
        }

        if (mInterrupted) {
            mInterrupted = false;
            return std::nullopt;
        }

        T ret = mQueue.front();
        mQueue.pop();
        return ret;
    }

    std::optional<T> WaitAndFront()
    {
        std::unique_lock lock(mMutex);
        while (mQueue.empty() && !mInterrupted) {
            mCondVar.wait(lock);
        }

        if (mInterrupted) {
            mInterrupted = false;
            return std::nullopt;
        }

        return mQueue.front();
    }

    void InterruptWaiting()
    {
        mCondVar.notify_all();
        mInterrupted = true;
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

    bool mInterrupted = false;
};


#endif //BLOCKINGQUEUE_H
