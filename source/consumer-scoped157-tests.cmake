add_executable(consumer-scoped157-tests tests/consumer-scoped157-tests.cpp)
target_include_directories(consumer-scoped157-tests BEFORE PRIVATE . tests external/safetyhook/include)
target_compile_options(consumer-scoped157-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME consumer-scoped157 COMMAND consumer-scoped157-tests)
