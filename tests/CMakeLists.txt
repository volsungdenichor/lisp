include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(lisp_tests basic_test.cpp ${LISP_SRC})
include_directories(
    "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(lisp_tests gtest_main gmock_main)
add_test(NAME lisp_tests COMMAND lisp_tests)
