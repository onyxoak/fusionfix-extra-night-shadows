add_executable(fresh-window49-tests tests/fresh-window49-tests.cpp)
target_include_directories(fresh-window49-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(fresh-window49-tests PRIVATE /W4 /WX /UNDEBUG)
add_test(NAME fresh-window49 COMMAND fresh-window49-tests)
