add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/live-reach122-extracted.inc"
 COMMAND "${SETTINGS117_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/Extract-LiveReach122.py" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
 DEPENDS standalone-runtime.cpp core/ShadowAllocationRuntime.inl core/standalone-shadow-core.cpp tests/Extract-LiveReach122.py VERBATIM)
add_executable(live-reach122-tests tests/live-reach122-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/live-reach122-extracted.inc")
target_include_directories(live-reach122-tests BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" .)
target_compile_options(live-reach122-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME live-reach122 COMMAND live-reach122-tests)
