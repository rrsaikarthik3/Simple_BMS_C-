project(BMS)

cmake_minimum_required (VERSION 3.11.3)
# set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CXX_FLAGS} -std=c++17 -pthread")

file(GLOB sources src/*.cpp) #src/*.h



include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(BMS ${sources})
target_link_libraries(BMS)