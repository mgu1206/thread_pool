cmake_minimum_required(VERSION 3.28)

project(thread_worker_sample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sample executable
add_executable(sample sample.cpp)

# Link with thread_worker library
target_link_libraries(sample PRIVATE thread_worker)

# Platform-specific compiler options
if(MSVC)
  target_compile_options(
    sample
    PRIVATE /EHsc # Enable C++ exception handling
            /W3 # Set warning level to 3
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(
    sample
    PRIVATE -Wall # Enable most warnings
            -Wextra # Enable extra warnings
            -Wpedantic # Strict ISO C++ compliance warnings
  )
endif()

# Link pthread on Unix-like systems
if(UNIX)
  target_link_libraries(sample PRIVATE pthread)
endif()
