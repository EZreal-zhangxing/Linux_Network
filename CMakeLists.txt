cmake_minimum_required(VERSION 3.20)

project(socketApp)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-Wall -O2 -g")

message(STATUS "project dir:${PROJECT_SOURCE_DIR}")

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
# aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_FILES)

message(STATUS "src files : ${SRC_FILES}")

include_directories(${PROJECT_SOURCE_DIR}/include)


add_executable(main ${SRC_FILES})

target_link_libraries(main -lpthread)