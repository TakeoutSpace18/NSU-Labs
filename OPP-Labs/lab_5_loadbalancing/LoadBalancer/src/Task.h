#ifndef TASK_H
#define TASK_H

constexpr int TASKS_TAG = 888;

class Task {
public:
    Task() = default;
    explicit Task(int timeMs) : mTimeMs(timeMs) {};

    void Execute() const
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(mTimeMs));
    }

private:
    int mTimeMs;
};



#endif //TASK_H
