cmake_minimum_required(VERSION 3.5)

project(Reseau)

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-std=c99 -Wall -pthread -D _DEFAULT_SOURCE")

# Libraries
add_library(queue src/queue.c)
add_library(utils src/utils.c)
add_library(request_functions src/request_functions.c)
add_library(processing src/processing.c)
add_library(cmd src/cmd.c)

# Links libraries
target_link_libraries(utils LINK_PUBLIC queue)
target_link_libraries(request_functions LINK_PUBLIC utils)
target_link_libraries(processing LINK_PUBLIC utils)
target_link_libraries(processing LINK_PUBLIC request_functions)

# Tracker
add_executable(tracker src/tracker.c)
target_link_libraries(tracker LINK_PUBLIC processing)
target_link_libraries(tracker LINK_PUBLIC cmd)

# Tests
add_executable(test_queue tst/test_queue.c)
add_executable(test_utils tst/test_utils.c)
add_executable(test_request_functions tst/test_request_functions.c)

# Links tests
target_link_libraries(test_queue LINK_PUBLIC queue)
target_link_libraries(test_utils LINK_PUBLIC utils)
target_link_libraries(test_request_functions LINK_PUBLIC request_functions)

# Make check
add_custom_target(check make COMMAND test_utils COMMAND test_request_functions
                             COMMAND test_queue)

# Make valgrind
add_custom_target(valgrind make COMMAND valgrind ./test_utils COMMAND valgrind ./test_request_functions
                                COMMAND valgrind ./test_queue)
