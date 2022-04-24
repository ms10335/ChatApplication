cmake_minimum_required(VERSION 3.21)
project(chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}-server server.cpp)
add_executable(${PROJECT_NAME}-client client.cpp)

target_compile_options(${PROJECT_NAME}-server PRIVATE -Wall -Wextra -Werror -Wpedantic)
target_compile_options(${PROJECT_NAME}-client PRIVATE -Wall -Wextra -Werror -Wpedantic)