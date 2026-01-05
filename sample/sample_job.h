#pragma once

#include "job.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>

// Concrete job implementation for demonstration
class sample_job : public job
{
public:
    sample_job(unsigned long long job_id,
               job_priority priority,
               const std::string& task_name,
               int work_duration_ms = 100)
        : job(job_id, priority, [](callback_data*) {})
        , _task_name(task_name)
        , _work_duration_ms(work_duration_ms)
    {
    }

    void work() override
    {
        std::stringstream ss;
        ss << "[Job #" << _job_id
           << " '" << _task_name << "' Priority:" << priorityToString(getJobPriority())
           << "] Started on thread " << std::this_thread::get_id() << std::endl;
        std::cout << ss.str();

        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(_work_duration_ms));

        ss.str("");
        ss << "[Job #" << _job_id
           << " '" << _task_name << "' Priority:" << priorityToString(getJobPriority())
           << "] Completed" << std::endl;
        std::cout << ss.str();
    }

private:
    std::string _task_name;
    int _work_duration_ms;

    std::string priorityToString(job_priority p)
    {
        switch (p)
        {
            case job_priority::HIGH_PRIORITY: return "HIGH";
            case job_priority::NORMAL_PRIORITY: return "NORMAL";
            case job_priority::LOW_PRIORITY: return "LOW";
            default: return "UNKNOWN";
        }
    }
};
