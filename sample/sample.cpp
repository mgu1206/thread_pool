#include <iostream>
#include <memory>

#include "sample_job.h"
#include "thread_pool.h"
#include "thread_worker.h"

void printSeparator(const std::string& title)
{
    std::cout << "\n========== " << title << " ==========\n" << std::endl;
}

int main()
{
    std::cout << "Thread Pool Sample Application\n" << std::endl;

    {
        // Create thread pool
        auto pool = std::make_shared<thread_pool>();

        printSeparator("Example 1: Basic Usage with Normal Priority Workers");

        // Add 3 normal priority workers
        for (int i = 0; i < 3; i++)
        {
            auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
            pool->addWorker(worker);
        }

        pool->setWorkersPriorityNumbers();

        std::cout << "Created thread pool with " << pool->getWorkerNumbers() << " workers" << std::endl;
        std::cout << "Adding 5 normal priority jobs..." << std::endl;

        // Add normal priority jobs
        for (int i = 1; i <= 5; i++)
        {
            auto job = std::make_shared<sample_job>(
                i,
                job_priority::NORMAL_PRIORITY,
                "Task-" + std::to_string(i),
                200  // 200ms work duration
            );
            pool->addJob(job);
        }

        // Wait for jobs to complete
        pool->stopPool(true);
    }

    printSeparator("Example 2: Mixed Priority Jobs");

    {
        // Create thread pool
        auto pool = std::make_shared<thread_pool>();

        for (int i = 0; i < 3; i++)
        {
            auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
            pool->addWorker(worker);
        }

        std::cout << "Adding mixed priority jobs (High, Normal, Low)..." << std::endl;

        // Add high priority job
        auto high_job = std::make_shared<sample_job>(
            100,
            job_priority::HIGH_PRIORITY,
            "HighPriority-Task",
            300
        );
        pool->addJob(high_job);

        // Add normal priority jobs
        for (int i = 1; i <= 3; i++)
        {
            auto normal_job = std::make_shared<sample_job>(
                200 + i,
                job_priority::NORMAL_PRIORITY,
                "NormalPriority-Task-" + std::to_string(i),
                200
            );
            pool->addJob(normal_job);
        }

        // Add low priority job
        auto low_job = std::make_shared<sample_job>(
            300,
            job_priority::LOW_PRIORITY,
            "LowPriority-Task",
            150
        );
        pool->addJob(low_job);

        // Wait for jobs to complete
        pool->stopPool(true);
    }

    printSeparator("Example 3: Adding High Priority Worker");

    {
        // Create thread pool
        auto pool = std::make_shared<thread_pool>();

        // Add high priority worker
        auto high_worker = std::make_shared<thread_worker>(job_priority::HIGH_PRIORITY);
        pool->addWorker(high_worker);
        pool->setWorkersPriorityNumbers();

        std::cout << "Now pool has " << pool->getWorkerNumbers() << " workers" << std::endl;
        std::cout << "Adding high and normal priority jobs..." << std::endl;

        // Add jobs
        for (int i = 1; i <= 3; i++)
        {
            auto h_job = std::make_shared<sample_job>(
                400 + i,
                job_priority::HIGH_PRIORITY,
                "HighPriority-Task-" + std::to_string(i),
                150
            );
            pool->addJob(h_job);
        }

        for (int i = 1; i <= 3; i++)
        {
            auto n_job = std::make_shared<sample_job>(
                500 + i,
                job_priority::NORMAL_PRIORITY,
                "NormalPriority-Task-" + std::to_string(i),
                150
            );
            pool->addJob(n_job);
        }

        // Wait for jobs to complete
        pool->stopPool(true);
    }

    printSeparator("Example 4: Adding Low Priority Worker");

    {
        // Create thread pool
        auto pool = std::make_shared<thread_pool>();

        // Add low priority worker
        auto low_worker = std::make_shared<thread_worker>(job_priority::LOW_PRIORITY);
        pool->addWorker(low_worker);
        pool->setWorkersPriorityNumbers();

        std::cout << "Now pool has " << pool->getWorkerNumbers() << " workers" << std::endl;
        std::cout << "Adding jobs of all priorities..." << std::endl;

        // Add all priority jobs
        auto job_h = std::make_shared<sample_job>(600, job_priority::HIGH_PRIORITY, "High", 200);
        auto job_n = std::make_shared<sample_job>(601, job_priority::NORMAL_PRIORITY, "Normal", 200);
        auto job_l = std::make_shared<sample_job>(602, job_priority::LOW_PRIORITY, "Low", 200);

        pool->addJob(job_h);
        pool->addJob(job_n);
        pool->addJob(job_l);

        // Wait for jobs to complete
        pool->stopPool(true);
    }

    std::cout << "\nSample completed!" << std::endl;

    return 0;
}
