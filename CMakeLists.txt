cmake_minimum_required(VERSION 3.10)
project(p2p-proto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads)
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
add_executable(protocole 
    main.cpp
    src/Supervisor.cpp
    src/UI.cpp
    src/TCPConnector.cpp
    src/NetworkHandler.cpp
    )
