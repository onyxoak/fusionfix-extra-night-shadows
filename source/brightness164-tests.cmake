add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/headlight164-retain.inc" "${CMAKE_CURRENT_BINARY_DIR}/headlight164-hook.inc"
 COMMAND "${PERF162_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/Extract-Brightness164.py" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
 DEPENDS standalone-runtime.cpp core/HeadlightEnhancementRuntime.inl tests/Extract-Brightness164.py VERBATIM)
add_executable(brightness164-tests tests/brightness164-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/headlight164-retain.inc" "${CMAKE_CURRENT_BINARY_DIR}/headlight164-hook.inc")
target_include_directories(brightness164-tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" . core external/safetyhook/include)
target_compile_options(brightness164-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME brightness164 COMMAND brightness164-tests)
