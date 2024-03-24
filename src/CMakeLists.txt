set(TARGET_NAME lisp)

add_executable(${TARGET_NAME} main.cpp ${LISP_SRC})

include_directories(
    " ${PROJECT_SOURCE_DIR}/include"
)