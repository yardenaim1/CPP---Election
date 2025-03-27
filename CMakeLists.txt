# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(CPP_Election LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Gather all .cpp files in src/ directory
file(GLOB SOURCES "src/*.cpp")
# Add the main executable
add_executable(main ${SOURCES})

# Include directories for header files
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add Google Test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Ensure consistent runtime library
add_subdirectory(external/googletest)

# Add the test executable
add_executable(my_tests
    tests/test_plus.cpp
    src/file_for_test.cpp
)
target_include_directories(my_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link Google Test to the test executable
target_link_libraries(my_tests gtest gtest_main)

message(STATUS "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CMAKE DEBUG PRINT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
