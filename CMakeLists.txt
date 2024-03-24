cmake_minimum_required(VERSION 3.5)
project(lisp)

enable_testing()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror -Wfatal-errors -pedantic")

set(LISP_SRC_ROOT "${PROJECT_SOURCE_DIR}/src/lisp")

set(LISP_SRC
    ${LISP_SRC_ROOT}/category.cpp
    ${LISP_SRC_ROOT}/value.cpp
    ${LISP_SRC_ROOT}/evaluate.cpp
    ${LISP_SRC_ROOT}/tokenizer.cpp
    ${LISP_SRC_ROOT}/parser.cpp
)

add_subdirectory(src)
add_subdirectory(tests)