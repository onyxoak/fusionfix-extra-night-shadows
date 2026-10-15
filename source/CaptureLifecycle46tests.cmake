add_executable(capture-lifecycle46-test tests/capture-lifecycle46.cpp)
target_compile_features(capture-lifecycle46-test PRIVATE cxx_std_20)
add_test(NAME capture-lifecycle46 COMMAND capture-lifecycle46-test)
