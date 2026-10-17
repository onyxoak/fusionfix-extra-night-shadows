set(PERF162_PYTHON "${Python3_EXECUTABLE}")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/clock-config162-extracted.inc" "${CMAKE_CURRENT_BINARY_DIR}/world-callbacks162-extracted.inc" "${CMAKE_CURRENT_BINARY_DIR}/world-publish162-extracted.inc"
 COMMAND "${PERF162_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/Extract-Performance162.py" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
 DEPENDS standalone-runtime.cpp WorldGeometryCatalogRuntime20.inc tests/Extract-Performance162.py VERBATIM)
add_executable(clock-config162-tests tests/clock-config162-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/clock-config162-extracted.inc")
target_include_directories(clock-config162-tests BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" . core)
target_compile_options(clock-config162-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME clock-config162 COMMAND clock-config162-tests)
add_executable(world-counters162-tests tests/world-counters162-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/world-callbacks162-extracted.inc" "${CMAKE_CURRENT_BINARY_DIR}/world-publish162-extracted.inc")
target_include_directories(world-counters162-tests BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" . core external/safetyhook/include)
target_compile_options(world-counters162-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME world-counters162 COMMAND world-counters162-tests)
