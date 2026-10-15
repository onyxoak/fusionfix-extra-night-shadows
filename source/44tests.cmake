add_executable(receiver-consequence44-tests tests/receiver-consequence44-tests.cpp)
target_include_directories(receiver-consequence44-tests PRIVATE .)
target_compile_options(receiver-consequence44-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME receiver-consequence44 COMMAND receiver-consequence44-tests)
