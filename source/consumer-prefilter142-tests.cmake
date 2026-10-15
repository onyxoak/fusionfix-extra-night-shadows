add_executable(consumer-prefilter142-tests tests/consumer-prefilter142-tests.cpp)
target_include_directories(consumer-prefilter142-tests BEFORE PRIVATE . tests external/safetyhook/include)
target_compile_options(consumer-prefilter142-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME consumer-prefilter142 COMMAND consumer-prefilter142-tests)
