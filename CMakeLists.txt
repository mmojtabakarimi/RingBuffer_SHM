cmake_minimum_required(VERSION 3.5)
project(RingBufferProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Add including debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


# Add include directories
include_directories(include)

# Add executable for the producer
add_executable(producer src/producer.cpp)
target_link_libraries(producer pthread rt)

# Add executable for the consumer
add_executable(consumer src/consumer.cpp)
target_link_libraries(consumer pthread rt)

# Find GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add executable for the tests
add_executable(ring_buffer_test test/ring_buffer_test.cpp)
target_link_libraries(ring_buffer_test ${GTEST_LIBRARIES} pthread rt)


# Add test target
enable_testing()
add_test(NAME RingBufferTest COMMAND ring_buffer_test)
