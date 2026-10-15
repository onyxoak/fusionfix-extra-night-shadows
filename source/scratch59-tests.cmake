add_executable(queued-scratch59-tests tests/queued-scratch59-tests.cpp)
target_include_directories(queued-scratch59-tests PRIVATE . "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_options(queued-scratch59-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME queued-scratch59 COMMAND queued-scratch59-tests)
