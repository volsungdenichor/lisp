set(TARGET_NAME lisp)

set(LISP_SRC
    "${PROJECT_SOURCE_DIR}/src/lisp/category.cpp"
)

add_executable(${TARGET_NAME} main.cpp ${LISP_SRC})

include_directories(
    " ${PROJECT_SOURCE_DIR}/include "
)