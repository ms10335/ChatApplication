cmake_minimum_required(VERSION 3.21)
project(chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME}-server server.cpp)
add_executable(${PROJECT_NAME}-client client.cpp)

target_compile_options(${PROJECT_NAME}-server PRIVATE -Wall -Wextra -Werror -Wpedantic)
target_compile_options(${PROJECT_NAME}-client PRIVATE -Wall -Wextra -Werror -Wpedantic)

enable_testing()

add_executable(
  ${PROJECT_NAME}-gt
  testing.cpp
)
target_link_libraries(
  ${PROJECT_NAME}-gt
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}-gt)