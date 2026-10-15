add_executable(native-copy-guard70-tests tests/native-copy-guard70-tests.cpp)
target_include_directories(native-copy-guard70-tests PRIVATE .)
target_compile_options(native-copy-guard70-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME native-copy-guard70 COMMAND native-copy-guard70-tests)
