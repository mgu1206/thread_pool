# Thread Worker - Priority-Based Thread Pool Library

A C++20 thread pool library implementing priority-based job scheduling with worker threads.

## Features

- **Priority-based job scheduling** (HIGH, NORMAL, LOW)
- **Flexible worker configuration** - Workers can be configured to handle specific priority jobs
- **Cross-platform support** - Windows (MSVC), macOS (Clang), Linux (GCC)
- **Thread-safe** - Uses mutexes and condition variables for synchronization
- **Smart pointer based** - Modern C++ memory management

## Project Structure

```
thread_worker/
├── CMakeLists.txt           # Main build configuration
├── callback_data.{h,cpp}    # Job callback data
├── job.{h,cpp}              # Abstract job base class
├── job_data.h               # Job input data base class
├── job_manager.{h,cpp}      # Job queue management
├── thread_pool.{h,cpp}      # Thread pool manager
├── thread_worker.{h,cpp}    # Worker thread implementation
└── sample/                  # Sample application
    ├── CMakeLists.txt       # Sample build configuration
    ├── sample.cpp           # Sample application code
    └── sample_job.h         # Sample job implementation
```

## Components

- `thread_pool` - Main thread pool manager
- `thread_worker` - Individual worker threads with priority affinity
- `job_manager` - Job queue management with priority queues
- `job` - Abstract base class for jobs (inherit to create custom jobs)
- `job_data` - Base class for job input data
- `callback_data` - Base class for job callback data

## Building

### Prerequisites

- CMake 3.28 or higher
- C++20 compatible compiler:
  - MSVC 2019+ (Windows)
  - Clang 10+ (macOS/Linux)
  - GCC 10+ (Linux)

### Build Instructions

#### macOS / Linux

```bash
mkdir build
cd build
cmake ..
make
```

#### Windows (Visual Studio)

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

This will build:
- `thread_worker` - Static library
- `sample` - Sample executable demonstrating library usage (in `sample/` subdirectory)

### Building Only the Library

If you only want to build the library without the sample:

```bash
mkdir build
cd build
cmake ..
cmake --build . --target thread_worker
```

## Running the Sample

After building, run the sample executable from the build directory:

```bash
# macOS / Linux
cd build
./sample/sample

# Windows
cd build
.\sample\Release\sample.exe
```

## Usage Example

```cpp
#include "thread_pool.h"
#include "thread_worker.h"
#include "sample_job.h"

int main()
{
    // Create thread pool
    auto pool = std::make_shared<thread_pool>();

    // Add workers with different priorities
    auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
    pool->addWorker(worker);

    pool->setWorkersPriorityNumbers();

    // Create and add jobs
    auto job = std::make_shared<sample_job>(
        1,                              // job_id
        job_priority::NORMAL_PRIORITY,  // priority
        "MyTask",                       // task name
        200                             // work duration in ms
    );

    pool->addJob(job);

    // Wait for completion
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Shutdown pool
    pool->stopPool(true, std::chrono::seconds(5));

    return 0;
}
```

## Creating Custom Jobs

Inherit from the `job` base class and implement the `work()` method:

```cpp
class my_custom_job : public job
{
public:
    my_custom_job(unsigned long long job_id, job_priority priority)
        : job(job_id, priority, [](callback_data*) {})
    {
    }

    void work() override
    {
        // Your job logic here
        std::cout << "Doing work..." << std::endl;
    }
};
```

## Priority Scheduling

The thread pool supports three priority levels:

- **HIGH_PRIORITY** - Processed first by high-priority workers, then normal workers
- **NORMAL_PRIORITY** - Processed by all workers
- **LOW_PRIORITY** - Processed last, mainly by low-priority workers

### Worker Priority Matching

- **High Priority Workers**: Process HIGH → NORMAL jobs
- **Normal Priority Workers**: Process NORMAL → LOW → HIGH jobs
- **Low Priority Workers**: Process LOW → NORMAL → HIGH jobs

## API Reference

### thread_pool

```cpp
void addWorker(std::shared_ptr<thread_worker> worker);
void removeWorker(std::shared_ptr<thread_worker> worker);
void addJob(std::shared_ptr<job> new_job);
void stopPool(bool wait_for_finish_jobs = false, std::chrono::seconds max_wait_time = std::chrono::seconds(0));
int getWorkerNumbers();
void setWorkersPriorityNumbers();
```

### thread_worker

```cpp
thread_worker(job_priority priority = job_priority::NORMAL_PRIORITY);
void startWorker();
void stopWorker();
job_priority getPriority();
```

### job_manager

```cpp
void push_job(std::shared_ptr<job> new_job);
std::shared_ptr<job> pop_job(std::vector<job_priority> job_priorities);
int getAllJobCount();
int getJobCount(std::vector<job_priority> job_priorities);
```

## Future Updates

The following improvements are planned for future releases:

### Code Modernization

- **Remove raw pointer usage** - Eliminate all raw pointers in favor of smart pointers
- **Modern C++ improvements** - Update codebase to fully utilize modern C++ features and best practices
- **Replace class-based data structures** - Use `std::variant` and `std::monostate` instead of inheritance-based `job_data` and `callback_data` classes for better type safety and performance

### Feature Enhancements

- **Watchdog for starvation prevention** - Implement watchdog mechanism to detect and prevent job starvation
- **Lambda job support** - Add support for lambda functions as jobs for easier inline task definition
- **Job chain support** - Implement job chaining capabilities to define dependent task sequences

## License

This project is provided as-is for educational and commercial use.
