#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <sstream>

#include "thread_pool.h"
#include "thread_worker.h"

void printSeparator(const std::string& title)
{
    std::cout << "\n========== " << title << " ==========\n" << std::endl;
}

int main()
{
    std::cout << "Lambda Job Sample Application\n" << std::endl;

    printSeparator("Example 1: Simple Lambda Jobs");
    {
        auto pool = std::make_shared<thread_pool>();

        // Add 3 normal priority workers
        for (int i = 0; i < 3; i++)
        {
            auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
            pool->addWorker(worker);
        }
        pool->setWorkersPriorityNumbers();

        std::cout << "Created thread pool with 3 workers\n" << std::endl;
        std::cout << "Adding 5 simple lambda jobs..." << std::endl;

        // Create simple lambda jobs
        for (int i = 1; i <= 5; i++)
        {
            auto job = std::make_shared<::job>(
                i,
                job_priority::NORMAL_PRIORITY,
                [i]() {
                    std::stringstream ss;
                    ss << "[Lambda Job #" << i << "] Started on thread "
                       << std::this_thread::get_id() << std::endl;
                    std::cout << ss.str();

                    std::this_thread::sleep_for(std::chrono::milliseconds(100));

                    ss.str("");
                    ss << "[Lambda Job #" << i << "] Completed" << std::endl;
                    std::cout << ss.str();
                }
            );
            pool->addJob(job);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        pool->stopPool(true, std::chrono::seconds(5));
    }

    printSeparator("Example 2: Lambda Jobs with Captures");
    {
        auto pool = std::make_shared<thread_pool>();
        auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
        pool->addWorker(worker);
        pool->setWorkersPriorityNumbers();

        std::atomic<int> counter{0};

        std::cout << "Creating jobs that increment a shared counter..." << std::endl;

        for (int i = 1; i <= 5; i++)
        {
            auto job = std::make_shared<::job>(
                100 + i,
                job_priority::NORMAL_PRIORITY,
                [&counter, i]() {
                    counter++;
                    std::cout << "[Job " << i << "] Counter incremented to: "
                             << counter << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            );
            pool->addJob(job);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        pool->stopPool(true, std::chrono::seconds(5));

        std::cout << "\nFinal counter value: " << counter << std::endl;
    }

    printSeparator("Example 3: Mixed Priority Lambda Jobs");
    {
        auto pool = std::make_shared<thread_pool>();

        // Add workers with different priorities
        auto high_worker = std::make_shared<thread_worker>(job_priority::HIGH_PRIORITY);
        auto normal_worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
        auto low_worker = std::make_shared<thread_worker>(job_priority::LOW_PRIORITY);

        pool->addWorker(normal_worker);
        pool->addWorker(high_worker);
        pool->addWorker(low_worker);
        pool->setWorkersPriorityNumbers();

        std::cout << "Adding jobs with different priorities..." << std::endl;

        // Add high priority job
        auto high_job = std::make_shared<::job>(
            200,
            job_priority::HIGH_PRIORITY,
            []() {
                std::cout << "[HIGH PRIORITY] Critical task executing on thread "
                         << std::this_thread::get_id() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                std::cout << "[HIGH PRIORITY] Critical task completed" << std::endl;
            }
        );

        // Add normal priority job
        auto normal_job = std::make_shared<::job>(
            201,
            job_priority::NORMAL_PRIORITY,
            []() {
                std::cout << "[NORMAL PRIORITY] Standard task executing on thread "
                         << std::this_thread::get_id() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                std::cout << "[NORMAL PRIORITY] Standard task completed" << std::endl;
            }
        );

        // Add low priority job
        auto low_job = std::make_shared<::job>(
            202,
            job_priority::LOW_PRIORITY,
            []() {
                std::cout << "[LOW PRIORITY] Background task executing on thread "
                         << std::this_thread::get_id() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                std::cout << "[LOW PRIORITY] Background task completed" << std::endl;
            }
        );

        // Add in reverse priority order to show prioritization
        pool->addJob(low_job);
        pool->addJob(normal_job);
        pool->addJob(high_job);

        std::this_thread::sleep_for(std::chrono::milliseconds(700));
        pool->stopPool(true, std::chrono::seconds(5));
    }

    printSeparator("Example 4: Lambda Jobs with Complex Captures");
    {
        auto pool = std::make_shared<thread_pool>();
        auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
        pool->addWorker(worker);
        pool->setWorkersPriorityNumbers();

        std::cout << "Creating jobs that process data..." << std::endl;

        std::vector<int> data = {10, 20, 30, 40, 50};
        std::atomic<int> sum{0};

        for (size_t i = 0; i < data.size(); i++)
        {
            int value = data[i];
            auto job = std::make_shared<::job>(
                300 + i,
                job_priority::NORMAL_PRIORITY,
                [value, &sum]() {
                    std::cout << "Processing value: " << value << std::endl;
                    sum += value;
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            );
            pool->addJob(job);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        pool->stopPool(true, std::chrono::seconds(5));

        std::cout << "\nSum of processed values: " << sum << std::endl;
    }

    printSeparator("Shutting Down");
    std::cout << "All lambda job samples completed successfully!\n" << std::endl;

    return 0;
}
