# Thread Worker - Priority-Based Thread Pool Library

A C++20 thread pool library implementing priority-based job scheduling with worker threads.

## Features

- **Priority-based job scheduling** (HIGH, NORMAL, LOW)
- **Flexible worker configuration** - Workers can be configured to handle specific priority jobs
- **Lambda job support** - Create jobs inline without inheritance using C++20 lambda functions
- **Future-based async execution** - Submit tasks and get results via `std::future` with full type safety
- **Cross-platform support** - Windows (MSVC), macOS (Clang), Linux (GCC)
- **Thread-safe** - Uses mutexes and condition variables for synchronization
- **Smart pointer based** - Modern C++ memory management with smart pointers throughout

## Project Structure

```
thread_pool/
├── CMakeLists.txt           # Main build configuration
├── src/                     # Library source code
│   ├── job.{h,cpp}              # Abstract job base class
│   ├── job_manager.{h,cpp}      # Job queue management
│   ├── thread_pool.{h,cpp}      # Thread pool manager
│   └── thread_worker.{h,cpp}    # Worker thread implementation
└── sample/                  # Sample applications
    ├── CMakeLists.txt           # Sample build configuration
    ├── sample.cpp               # Traditional inheritance-based jobs
    ├── sample_lambda.cpp        # Lambda-based jobs
    ├── future_sample.cpp        # Future-based async job submission
    ├── test_return_values.cpp   # Return value handling
    └── sample_job.h             # Sample job implementation
```

## Components

- `thread_pool` - Main thread pool manager with support for lambda jobs and future-based async execution
- `thread_worker` - Individual worker threads with priority affinity
- `job_manager` - Job queue management with priority queues
- `job` - Abstract base class for jobs (supports both inheritance and lambda-based jobs)

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
- `sample` - Sample executable demonstrating inheritance-based jobs
- `sample_lambda` - Sample executable demonstrating lambda-based jobs
- `future_sample` - Sample executable demonstrating future-based async job submission
- `test_return_values` - Sample executable demonstrating return value handling

### Building Only the Library

If you only want to build the library without the sample:

```bash
mkdir build
cd build
cmake ..
cmake --build . --target thread_worker
```

## Running the Samples

After building, run the sample executables from the build directory:

### Traditional Inheritance Pattern Sample

```bash
# macOS / Linux
./build/sample/sample

# Windows
.\build\sample\Release\sample.exe
```

### Lambda Jobs Sample

```bash
# macOS / Linux
./build/sample/sample_lambda

# Windows
.\build\sample\Release\sample_lambda.exe
```

### Future-Based Async Job Submission Sample

```bash
# macOS / Linux
./build/sample/future_sample

# Windows
.\build\sample\Release\future_sample.exe
```

### Return Values Test Sample

```bash
# macOS / Linux
./build/sample/test_return_values

# Windows
.\build\sample\Release\test_return_values.exe
```

## Usage Examples

### Simple Lambda Job Example

```cpp
#include "thread_pool.h"
#include "thread_worker.h"

int main()
{
    // Create thread pool
    auto pool = std::make_shared<thread_pool>();

    // Add workers
    auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
    pool->addWorker(worker);
    pool->setWorkersPriorityNumbers();

    // Create and add a lambda job
    auto job = std::make_shared<::job>(
        1,                              // job_id
        job_priority::NORMAL_PRIORITY,  // priority
        []() {                          // work lambda
            std::cout << "Task executed!" << std::endl;
        }
    );
    pool->addJob(job);

    // Shutdown and wait for completion
    pool->stopPool(true);

    return 0;
}
```

### Future-Based Async Execution Example

```cpp
#include "thread_pool.h"
#include "thread_worker.h"

int main()
{
    // Create thread pool
    auto pool = std::make_shared<thread_pool>();

    // Add workers
    for (int i = 0; i < 2; i++)
    {
        auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
        pool->addWorker(worker);
    }
    pool->setWorkersPriorityNumbers();

    // Submit a task and get a future
    auto future = pool->submit(
        job_priority::NORMAL_PRIORITY,
        [](int a, int b) {
            return a + b;
        },
        21,
        21
    );

    // Wait for result and print
    std::cout << "Result: " << future.get() << std::endl;

    // Shutdown pool
    pool->stopPool(true);

    return 0;
}
```

## Creating Custom Jobs

There are three ways to create and execute jobs:

### Option 1: Lambda Jobs - Recommended for Simple Tasks

Create jobs directly with lambda functions without inheritance:

```cpp
auto pool = std::make_shared<thread_pool>();
auto worker = std::make_shared<thread_worker>(job_priority::NORMAL_PRIORITY);
pool->addWorker(worker);
pool->setWorkersPriorityNumbers();

// Simple lambda job
auto job = std::make_shared<::job>(
    1,                              // job_id
    job_priority::HIGH_PRIORITY,    // priority
    []() {                          // work lambda
        std::cout << "Task executed!" << std::endl;
    }
);
pool->addJob(job);
```

Lambda jobs support captures for accessing external data:

```cpp
std::atomic<int> counter{0};

auto job = std::make_shared<::job>(
    2,
    job_priority::NORMAL_PRIORITY,
    [&counter]() {  // Capture counter by reference
        counter++;
        std::cout << "Counter: " << counter << std::endl;
    }
);
pool->addJob(job);
```

**Simplified constructors** are also available:

```cpp
// Auto job_id = 0, default priority = NORMAL
auto simple_job = std::make_shared<::job>([]() {
    std::cout << "Simple task!" << std::endl;
});
pool->addJob(simple_job);

// Specify priority only
auto priority_job = std::make_shared<::job>(
    job_priority::HIGH_PRIORITY,
    []() { std::cout << "High priority task!" << std::endl; }
);
pool->addJob(priority_job);
```

### Option 2: Future-Based Async Execution - For Getting Return Values

Use the `submit()` method to execute tasks and get results via `std::future`:

```cpp
// Submit with priority
auto future = pool->submit(
    job_priority::HIGH_PRIORITY,
    [](int a, int b) {
        return a * b;
    },
    6,
    7
);

std::cout << "Result: " << future.get() << std::endl;  // Output: 42

// Submit with default normal priority
auto future2 = pool->submit([]() {
    return "Hello from thread pool!";
});

std::string result = future2.get();
```

### Option 3: Class Inheritance - For Complex Jobs

Inherit from the `job` base class and implement the `work()` method:

```cpp
class my_custom_job : public job
{
public:
    my_custom_job(unsigned long long job_id, job_priority priority)
        : job(job_id)
    {
        setJobPriority(priority);
    }

    void work() override
    {
        // Your job logic here
        std::cout << "Doing work..." << std::endl;
    }
};

// Usage
auto job = std::make_shared<my_custom_job>(1, job_priority::HIGH_PRIORITY);
pool->addJob(job);
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
// Worker management
void addWorker(std::shared_ptr<thread_worker> worker);
void removeWorker(std::shared_ptr<thread_worker> worker);
void setWorkersPriorityNumbers();
int getWorkerNumbers();

// Job submission
void addJob(std::shared_ptr<job> new_job);

// Future-based async execution (returns std::future)
template <typename F, typename... Args>
auto submit(F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

template <typename F, typename... Args>
auto submit(job_priority priority, F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

// Pool control
void stopPool(bool wait_for_finish_jobs = false, std::chrono::seconds max_wait_time = std::chrono::seconds(0));
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

## Recent Updates

### Completed Features

- ✅ **Lambda job support** - Full support for lambda functions as jobs without requiring inheritance
- ✅ **Future-based async execution** - Added `submit()` method that returns `std::future` for getting task results
- ✅ **Simplified API** - Removed legacy `callback_data` and `job_data` classes for cleaner, more intuitive interface
- ✅ **Multiple sample applications** - Comprehensive examples demonstrating different usage patterns

## Future Enhancements

The following improvements are planned for future releases:

### Code Modernization

- **Remove raw pointer usage** - Eliminate remaining raw pointers in favor of smart pointers
- **Modern C++ improvements** - Continue updating codebase to fully utilize C++20 features and best practices

### Feature Enhancements

- **Watchdog for starvation prevention** - Implement watchdog mechanism to detect and prevent job starvation
- **Job chain support** - Implement job chaining capabilities to define dependent task sequences
- **Enhanced exception handling** - Improve error handling and reporting throughout the library

## License

This project is provided as-is for educational and commercial use.
