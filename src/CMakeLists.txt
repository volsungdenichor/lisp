set(TARGET_NAME lisp)

set(LISP_SRC
    "${PROJECT_SOURCE_DIR}/src/lisp/category.cpp"
    "${PROJECT_SOURCE_DIR}/src/lisp/value.cpp"
    "${PROJECT_SOURCE_DIR}/src/lisp/evaluate.cpp"
)

add_executable(${TARGET_NAME} main.cpp ${LISP_SRC})

include_directories(
    " ${PROJECT_SOURCE_DIR}/include"
)