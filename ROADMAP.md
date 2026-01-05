# Thread Pool Library - Future Roadmap

This document outlines planned improvements and feature enhancements for the thread pool library.

## Code Modernization

### ✅ Remove Raw Pointer Usage (COMPLETED)
- **Status**: ✅ Completed
- **Changes Made**:
  - Replaced `std::thread*` with `std::jthread` (C++20) in `thread_worker`
  - Replaced `job_data*` with `std::unique_ptr<job_data>` for ownership semantics
  - Replaced `callback_data*` with `std::shared_ptr<callback_data>` in callback functions
- **Benefits**:
  - Automatic resource management (RAII)
  - No manual `delete` or `join()` calls needed
  - Cooperative cancellation with `std::jthread`
  - Better type safety and ownership semantics

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
- **Objective**: Implement fluent job chaining API similar to C++ async/futures
- **Requirements**:
  - Chainable `.then()` API for sequential job dependencies
  - Jobs execute only after predecessor jobs complete
  - Automatic dependency resolution
  - Error propagation through chains
  - Return value passing between chained jobs
- **Usage Example**:
  ```cpp
  // Fluent chaining API (similar to std::async/promises)
  pool->addJob(task1)
      .then(task2)
      .then(task3);

  // With error handling
  pool->addJob(task1)
      .then(task2)
      .then(task3)
      .onError([](std::exception_ptr e) {
          // Handle errors from any task in chain
      });

  // Parallel branching from one task
  auto base = pool->addJob(task1);
  base.then(task2a);  // Branch 1
  base.then(task2b);  // Branch 2
  ```

## Priority Order

1. ~~**High Priority**: Remove raw pointer usage (improves safety and RAII compliance)~~ ✅ **COMPLETED**
2. **High Priority**: Lambda job support (immediate usability improvement)
3. **Medium Priority**: Modern C++ improvements (incremental quality improvements)
4. **Medium Priority**: Job chain support (significant feature addition)
5. **Low Priority**: Replace class-based data structures (optimization, requires major refactoring)
6. **Low Priority**: Watchdog for starvation prevention (nice-to-have for specific use cases)

## Completed Features

### Raw Pointer Elimination (v2.0)
**Completed**: 2026-01-06

All raw pointers have been replaced with modern C++20 smart pointers:
- `std::jthread` for thread management with automatic joining
- `std::unique_ptr<job_data>` for exclusive ownership of job data
- `std::shared_ptr<callback_data>` for shared callback data

This eliminates manual memory management and prevents resource leaks.
