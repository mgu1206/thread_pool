#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "thread_pool.h"
#include "thread_worker.h"

class MultiplyJob
{
public:
    int operator()(int a, int b) const
    {
        return a * b;
    }
};

int main()
{
    auto pool = std::make_shared<thread_pool>();

    for (int i = 0; i < 2; ++i)
    {
        auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
        pool->addWorker(worker);
    }

    pool->setWorkersPriorityNumbers();

    MultiplyJob job;
    auto class_future = pool->submit(job_priority::NORMAL_PRIORITY, job, 6, 7);

    auto lambda_future = pool->submit(job_priority::NORMAL_PRIORITY,
        []() {
            return std::string("thread_pool");
        });

    int class_result = class_future.get();
    std::string lambda_result = lambda_future.get();

    assert(class_result == 42);
    assert(lambda_result == "thread_pool");

    std::cout << "Class result: " << class_result << "\n";
    std::cout << "Lambda result: " << lambda_result << "\n";

    pool->stopPool(true);

    return 0;
}
