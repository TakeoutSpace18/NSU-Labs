#ifndef TASK_H
#define TASK_H


class Task {
public:
    explicit Task(int timeMs) : mTimeMs(timeMs) {};

    void execute() const
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(mTimeMs));
    }

private:
    int mTimeMs;
};



#endif //TASK_H
