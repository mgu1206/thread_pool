# Thread Worker Sample Application

This sample application demonstrates how to use the thread_worker library with various scenarios.

## What This Sample Demonstrates

1. **Basic Usage** - Creating a thread pool with normal priority workers
2. **Mixed Priority Jobs** - Submitting jobs with different priorities (HIGH, NORMAL, LOW)
3. **Dynamic Worker Management** - Adding workers with different priorities at runtime
4. **Priority-Based Scheduling** - How jobs are scheduled based on worker and job priorities
5. **Graceful Shutdown** - Properly stopping the thread pool and waiting for job completion

## Building

The sample is built automatically when you build the main project:

```bash
cd ..
mkdir build
cd build
cmake ..
make  # or: cmake --build .
```

## Running

From the build directory:

```bash
# macOS / Linux
./sample/sample

# Windows
.\sample\Release\sample.exe
```

## Files

- **sample.cpp** - Main sample application with 4 demonstration examples
- **sample_job.h** - Concrete implementation of the `job` abstract class
- **CMakeLists.txt** - Build configuration for the sample

## Creating Your Own Application

To create your own application using the thread_worker library:

1. Create a concrete job class by inheriting from `job`:

```cpp
#include "job.h"

class my_job : public job
{
public:
    my_job(unsigned long long id, job_priority priority)
        : job(id, priority, [](callback_data*) {})
    {
    }

    void work() override
    {
        // Your work logic here
    }
};
```

2. Create a thread pool and add workers:

```cpp
auto pool = std::make_shared<thread_pool>();
auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
pool->addWorker(worker);
pool->setWorkersPriorityNumbers();
```

3. Submit jobs to the pool:

```cpp
auto job = std::make_shared<my_job>(1, job_priority::NORMAL_PRIORITY);
pool->addJob(job);
```

4. Shutdown when done:

```cpp
pool->stopPool(true, std::chrono::seconds(5));
```

## Expected Output

The sample will print job execution information showing:
- Job ID and name
- Job priority
- Thread ID executing the job
- Start and completion messages

This demonstrates the priority-based scheduling in action.
