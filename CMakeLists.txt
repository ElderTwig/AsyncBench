cmake_minimum_required(VERSION 3.10)

project(Bench LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_VERSION VERSION_LESS 3.12)
    file(APPEND ${CMAKE_BINARY_DIR}/compile_commands.json "")
else()
    file(TOUCH ${CMAKE_BINARY_DIR}/compile_commands.json)
endif()

configure_file(${CMAKE_BINARY_DIR}/compile_commands.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    COPYONLY)

file(GLOB
  SRC
  *.cpp
  *.h)

add_executable(bench ${SRC})

find_package(Threads REQUIRED)

target_link_libraries(bench
  PRIVATE Threads::Threads)
