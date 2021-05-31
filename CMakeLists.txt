cmake_minimum_required(VERSION 3.15)
project(p2p-proto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# rozmowa o prace wait xddd.
find_package(Threads)
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

# target_include_directories((p2p-proto PRIVATE ))
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
add_executable(protocole 
    main.cpp
    src/server.cpp
    src/Supervisor.cpp
    src/ThreadManager.cpp
    src/UI.cpp)   
