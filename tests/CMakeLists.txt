cmake_minimum_required(VERSION 3.10)
project(hotel_tests)

# Find required packages
find_package(Criterion REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/ui
)

# Add test executable
add_executable(run_tests
    run_tests.c
    # Add more test files here
)

# Link with Criterion and our libraries
target_link_libraries(run_tests
    PRIVATE
    criterion
    # Link with your project's core libraries
    # -lhotel_ui
    # -lhotel_core
)

# Add test
add_test(NAME ui_tests COMMAND run_tests)
