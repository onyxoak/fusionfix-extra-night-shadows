set(PRESENT151_PYTHON "${Python3_EXECUTABLE}")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/present151-extracted.inc"
 COMMAND "${PRESENT151_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/Extract-Present151.py" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/present151-extracted.inc"
 DEPENDS PresentFrameTime151Runtime.inc tests/Extract-Present151.py VERBATIM)
add_executable(present-time151-tests tests/present-time151-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/present151-extracted.inc")
target_include_directories(present-time151-tests BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" . core)
target_compile_options(present-time151-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME present-time151 COMMAND present-time151-tests)
