add_executable(shortlist-helper48-tests tests/shortlist-helper48-tests.cpp)
target_include_directories(shortlist-helper48-tests PRIVATE .)
target_compile_options(shortlist-helper48-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME shortlist-helper48 COMMAND shortlist-helper48-tests)
