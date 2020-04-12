cmake_minimum_required(VERSION 3.5)

project(IRM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(IRM src/Main.cpp)

if(UNIX)
    target_link_libraries(IRM m)
endif (UNIX)

# https://stackoverflow.com/a/50882216
if(MSVC)
  target_compile_options(IRM PRIVATE /W4 /WX)
else()
  target_compile_options(IRM PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
