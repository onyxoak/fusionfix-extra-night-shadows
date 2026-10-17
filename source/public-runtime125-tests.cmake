set(PUBLIC125_PYTHON "${Python3_EXECUTABLE}")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/public-runtime125-extracted.inc"
 COMMAND "${PUBLIC125_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/Extract-PublicRuntime125.py" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/public-runtime125-extracted.inc"
 DEPENDS standalone-runtime.cpp police-nearby.inc traffic-signal-observer.inc RuntimeFeaturePolicy125.hpp PresetGeneration117.hpp PresetIni117.hpp tests/Extract-PublicRuntime125.py VERBATIM)
add_executable(public-runtime125-tests tests/public-runtime125-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/public-runtime125-extracted.inc")
target_include_directories(public-runtime125-tests BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" .)
target_compile_options(public-runtime125-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME public-runtime125 COMMAND public-runtime125-tests)
