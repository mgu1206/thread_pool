cmake_minimum_required(VERSION 3.28)

project(thread_worker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME})

# Platform-specific compiler options
if(MSVC)
  # MSVC (Windows Visual Studio)
  target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
  target_compile_options(
    ${PROJECT_NAME}
    PRIVATE /EHsc # Enable C++ exception handling
            /external:anglebrackets /external:W0 # ignore external warnings
            /W3 # Set warning level to 3
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # GCC (Linux) or Clang (macOS/Linux)
  target_compile_options(
    ${PROJECT_NAME}
    PRIVATE -Wall # Enable most warnings
            -Wextra # Enable extra warnings
            -Wpedantic # Strict ISO C++ compliance warnings
            -fexceptions # Enable exception handling (default, but explicit)
  )
endif()

# Set source files thread_worker
set(THREAD_WORKER_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/callback_data.h
    ${CMAKE_CURRENT_LIST_DIR}/job.h
    ${CMAKE_CURRENT_LIST_DIR}/job_data.h
    ${CMAKE_CURRENT_LIST_DIR}/job_manager.h
    ${CMAKE_CURRENT_LIST_DIR}/thread_pool.h
    ${CMAKE_CURRENT_LIST_DIR}/thread_worker.h)

set(THREAD_WORKER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/job.cpp ${CMAKE_CURRENT_LIST_DIR}/job_manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thread_pool.cpp
    ${CMAKE_CURRENT_LIST_DIR}/thread_worker.cpp)

# Add source file to the target
target_sources(${PROJECT_NAME} PRIVATE ${THREAD_WORKER_HEADERS}
                                       ${THREAD_WORKER_SOURCES})

# Set include directories for the library
target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add sample subdirectory
add_subdirectory(sample)
