#include <iostream>
#include <BlockingQueue.h>
#include <future>

int main()
{
    BlockingQueue<int> queue;

    auto f1 = std::async(std::launch::async, [&] { return queue.WaitAndFront(); });
    auto f2 = std::async(std::launch::async,[&] { return queue.WaitAndFront(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    queue.Push(123);

    std::cout << "f1 value: " << f1.get().value() << "\n";
    std::cout << "f2 value: " << f2.get().value() << "\n";

    queue.TryPop();

    auto f3 = std::async(std::launch::async, [&] { return queue.WaitAndPop(); });
    auto f4 = std::async(std::launch::async,[&] { return queue.WaitAndPop(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    queue.Push(456);
    queue.Push(789);

    std::cout << "f3 value: " << f3.get().value() << "\n";
    std::cout << "f4 value: " << f4.get().value() << "\n";

    return EXIT_SUCCESS;
}
