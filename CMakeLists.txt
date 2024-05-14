cmake_minimum_required (VERSION 3.13)

project (awsh)

add_subdirectory (ThirdParty/fmt)
set(CMAKE_CXX_STANDARD 20)

set (SOURCES 
    src/main.cpp
    src/Builtins.cpp)

add_executable (${PROJECT_NAME} ${SOURCES})

