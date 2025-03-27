# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(CPP_Election LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable target
add_executable(main
    civilian.cpp       
    district.cpp    
    electionround.cpp
    electionResults.cpp
    menufunctions.cpp
    party.cpp
    votecount.cpp  
    main.cpp
)

# Include directories for header files
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR})