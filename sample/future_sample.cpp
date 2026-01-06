#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "thread_pool.h"
#include "thread_worker.h"

int main()
{
    std::cout << "Future Sample Application" << std::endl;

    auto pool = std::make_shared<thread_pool>();

    for (int i = 0; i < 2; i++)
    {
        auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
        pool->addWorker(worker);
    }

    pool->setWorkersPriorityNumbers();

    auto future = pool->submit(job_priority::NORMAL_PRIORITY,
        [](int a, int b) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            return a + b;
        },
        21,
        21);

    std::cout << "Result from future: " << future.get() << std::endl;

    pool->stopPool(true);

    return 0;
}
