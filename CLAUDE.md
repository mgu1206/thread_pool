# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A C++20 thread pool library implementing priority-based job scheduling. The library uses three priority levels (HIGH, NORMAL, LOW) with workers that can be configured to handle specific priority jobs.

## Build Commands

### Standard Build (Library + Sample)

```bash
# macOS / Linux
mkdir build && cd build
cmake ..
make

# Windows (Visual Studio)
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Build Library Only

```bash
mkdir build && cd build
cmake ..
cmake --build . --target thread_worker
```

### Run Sample Application

```bash
# macOS / Linux
./build/sample/sample

# Windows
.\build\sample\Release\sample.exe
```

## Architecture

### Core Components

- **thread_pool** - Central manager that coordinates workers and jobs
  - Manages worker lifecycle (add/remove workers)
  - Dispatches jobs to the job_manager
  - Handles shutdown with optional wait-for-completion
  - Tracks worker priority distribution via `_priority_worker_numbers`

- **job_manager** - Priority-based job queue manager
  - Maintains separate queues per priority level in `_priority_job_list`
  - Implements priority-aware job popping based on worker preferences
  - Notifies workers when new jobs arrive via callback mechanism

- **thread_worker** - Worker thread with priority affinity
  - Each worker has a priority that determines job selection order
  - Uses `_job_match_priorities` vector to define job selection strategy
  - Sleeps on condition variable, wakes when matching jobs available
  - Raw pointer `_worker_thread` (planned for smart pointer migration)

- **job** - Abstract base class for work units
  - Virtual `work()` method can be overridden by subclasses or defined via lambda
  - Supports lambda-based inline work functions for simple tasks
  - Holds optional `job_data*` for input (raw pointer, planned for std::variant migration)

### Priority Matching System

Worker job selection priority (defined in `thread_worker::setJobMatchPriorities()`):

- **HIGH_PRIORITY workers**: HIGH → NORMAL
- **NORMAL_PRIORITY workers**: NORMAL → LOW → HIGH
- **LOW_PRIORITY workers**: LOW → NORMAL → HIGH

This ensures high-priority jobs get processed quickly while preventing starvation of lower-priority work.

### Threading Model

- Workers use condition variables (`_worker_condition`) for efficient sleep/wake
- `job_manager` holds callback to notify all workers via `thread_pool::notifyWakeUpWorkers()`
- Worker function (`thread_worker::worker_function()`) runs in loop:
  1. Wait on condition variable for matching jobs
  2. Pop highest priority matching job from manager
  3. Execute job's `work()` method
  4. Repeat until `_terminated` flag set

### Key Design Patterns

- **enable_shared_from_this**: Used by `thread_pool`, `job_manager`, and `job` to safely obtain `shared_ptr` from `this`
- **Weak pointers for back-references**: Workers hold `weak_ptr<job_manager>` to avoid circular dependencies
- **Mutex + Condition Variable**: Standard thread synchronization pattern for job queue access

## Creating Custom Jobs

### Lambda Jobs (C++20) - Recommended

Create jobs directly with lambda functions for simple, inline tasks:

```cpp
// Basic lambda job
auto job = std::make_shared<job>(
    1,                              // job_id
    job_priority::NORMAL_PRIORITY,  // priority
    []() {                          // work lambda
        std::cout << "Processing task..." << std::endl;
    }
);
pool->addJob(job);

// Lambda with captures
std::atomic<int> counter{0};
auto capture_job = std::make_shared<job>(
    2,
    job_priority::HIGH_PRIORITY,
    [&counter]() {
        counter++;
        std::cout << "Counter: " << counter << std::endl;
    }
);
pool->addJob(capture_job);

// Minimal lambda job (auto job_id = 0, default priority = NORMAL)
auto simple_job = std::make_shared<job>([]() {
    std::cout << "Simple task!" << std::endl;
});
pool->addJob(simple_job);
```

### Class Inheritance - For Complex Jobs

Inherit from `job` base class and implement the `work()` method for complex, multi-step operations:

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
        step1();
        step2();
        step3();
    }

private:
    void step1() { /* ... */ }
    void step2() { /* ... */ }
    void step3() { /* ... */ }
};
```

**When to use each approach:**
- **Lambda jobs**: Simple tasks, inline logic, quick prototyping, capturing external variables
- **Class inheritance**: Complex multi-step operations, reusable job types, jobs with significant state

## Important Notes

- Always call `thread_pool::setWorkersPriorityNumbers()` after adding/removing workers
- When shutting down, use `stopPool(true, timeout)` to wait for jobs to complete
- Workers automatically start when added to pool via `addWorker()`

## Modern C++20 Features

The library uses modern C++20 features for safety and performance:

### Smart Pointers
- **std::jthread**: Thread management with automatic joining and cooperative cancellation via `stop_token`
- **std::unique_ptr<job_data>**: Exclusive ownership of job input data (use `std::move()` when passing)

### Thread Shutdown Mechanism
Workers use C++20 `std::stop_token` for cooperative cancellation:
- `condition_variable_any` supports stop tokens for interruptible waits
- Proper shutdown order: `request_stop()` → `notify_all()` → `join()`
- Thread pool sets `_terminated` flag before stopping workers to prevent new operations
- Clean shutdown guaranteed without race conditions

Example creating a job with data:
```cpp
auto data = std::make_unique<my_job_data>();
auto job = std::make_shared<job>(
    job_id,
    job_priority::HIGH_PRIORITY,
    std::move(data),
    []() { /* work logic using data */ }
);
pool->addJob(job);
```
