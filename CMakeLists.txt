cmake_minimum_required(VERSION 3.9)
project(Browser-Engine)
find_package(Threads)

# Set compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(COVERAGE OFF CACHE BOOL "Coverage")

include(gtest.cmake)

# Include project headers
include_directories(./include)
# Define the source files and dependencies for the executable
set(SOURCE_FILES
    src/css.cpp
    src/dom.cpp
    src/layout.cpp
    src/parser.cpp
    src/parser_css.cpp
    src/parser_html.cpp
    src/printer.cpp
    src/style.cpp
)

set(TEST_FILES
    tests/css.cpp
    tests/html.cpp
    tests/layout.cpp
    tests/main.cpp
    tests/printer.cpp
    tests/style.cpp
)

add_executable(testing ${SOURCE_FILES} ${TEST_FILES})
target_compile_options(testing PRIVATE ${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pedantic -pedantic-errors -g)
target_link_libraries(testing PRIVATE gtest ${CMAKE_THREAD_LIBS_INIT})

if (COVERAGE)
    target_compile_options(testing PRIVATE --coverage)
    target_link_libraries(testing PRIVATE --coverage)
endif()