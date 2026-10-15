add_executable(frame-boundary154-tests tests/frame-boundary154-tests.cpp)
target_include_directories(frame-boundary154-tests BEFORE PRIVATE . core)
target_compile_options(frame-boundary154-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME frame-boundary154 COMMAND frame-boundary154-tests)
