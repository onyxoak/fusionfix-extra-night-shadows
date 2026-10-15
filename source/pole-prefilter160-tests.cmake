add_executable(pole-prefilter160-tests tests/pole-prefilter160-tests.cpp)
target_include_directories(pole-prefilter160-tests BEFORE PRIVATE . tests)
target_compile_options(pole-prefilter160-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME pole-prefilter160 COMMAND pole-prefilter160-tests)
