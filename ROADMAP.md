# Thread Pool Library - Future Roadmap

This document outlines planned improvements and feature enhancements for the thread pool library.

## Code Modernization

### Remove Raw Pointer Usage
- **Objective**: Eliminate all raw pointers in favor of smart pointers
- **Current Issues**:
  - `thread_worker::_worker_thread` uses `std::thread*` (raw pointer)
  - `job_data*` and `callback_data*` passed as raw pointers
- **Target**: Replace with `std::unique_ptr<std::thread>` and smart pointer-based data structures

### Modern C++ Improvements
- **Objective**: Update codebase to fully utilize modern C++ features and best practices
- **Planned Enhancements**:
  - Use `std::jthread` (C++20) instead of manual thread management
  - Leverage structured bindings where applicable
  - Apply constexpr and nodiscard attributes appropriately
  - Use concepts for template constraints
  - Improve move semantics usage

### Replace Class-Based Data Structures
- **Objective**: Use `std::variant` and `std::monostate` instead of inheritance-based `job_data` and `callback_data` classes
- **Benefits**:
  - Better type safety
  - Improved performance (no virtual function overhead)
  - Compile-time polymorphism
- **Target Classes**:
  - `job_data` base class → `std::variant`
  - `callback_data` base class → `std::variant`

## Feature Enhancements

### Watchdog for Starvation Prevention
- **Objective**: Implement watchdog mechanism to detect and prevent job starvation
- **Requirements**:
  - Monitor job queue wait times per priority level
  - Detect when low/normal priority jobs are starved by high priority jobs
  - Implement dynamic priority boosting for starved jobs
  - Configurable starvation threshold

### Lambda Job Support
- **Objective**: Add support for lambda functions as jobs for easier inline task definition
- **Usage Example**:
  ```cpp
  pool->addJob([](){
      std::cout << "Lambda job executed!" << std::endl;
  }, job_priority::NORMAL_PRIORITY);
  ```
- **Benefits**:
  - Eliminate need to create custom job classes for simple tasks
  - Reduce boilerplate code
  - More intuitive API

### Job Chain Support
- **Objective**: Implement job chaining capabilities to define dependent task sequences
- **Requirements**:
  - Define jobs that execute only after predecessor jobs complete
  - Support both linear chains and DAG (Directed Acyclic Graph) dependencies
  - Automatic dependency resolution
  - Error propagation through chains
- **Usage Example**:
  ```cpp
  auto job1 = pool->addJob(task1);
  auto job2 = pool->addJobAfter(job1, task2);
  auto job3 = pool->addJobAfter(job2, task3);
  ```

## Priority Order

1. **High Priority**: Remove raw pointer usage (improves safety and RAII compliance)
2. **High Priority**: Lambda job support (immediate usability improvement)
3. **Medium Priority**: Modern C++ improvements (incremental quality improvements)
4. **Medium Priority**: Job chain support (significant feature addition)
5. **Low Priority**: Replace class-based data structures (optimization, requires major refactoring)
6. **Low Priority**: Watchdog for starvation prevention (nice-to-have for specific use cases)
