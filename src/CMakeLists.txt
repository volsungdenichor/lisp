set(TARGET_NAME lisp)

set(LISP_SRC_ROOT "${PROJECT_SOURCE_DIR}/src/lisp")

set(LISP_SRC
    ${LISP_SRC_ROOT}/category.cpp
    ${LISP_SRC_ROOT}/value.cpp
    ${LISP_SRC_ROOT}/evaluate.cpp
    ${LISP_SRC_ROOT}/tokenizer.cpp
    ${LISP_SRC_ROOT}/parser.cpp
)

add_executable(${TARGET_NAME} main.cpp ${LISP_SRC})

include_directories(
    " ${PROJECT_SOURCE_DIR}/include"
)