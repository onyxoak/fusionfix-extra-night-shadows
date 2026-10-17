set(HEAD146_PYTHON "${Python3_EXECUTABLE}")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/head-capture146-extracted.inc"
 COMMAND "${HEAD146_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/Extract-HeadCapture146.py" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/head-capture146-extracted.inc"
 DEPENDS core/ShadowCasterRuntime.inl tests/Extract-HeadCapture146.py VERBATIM)
add_executable(head-capture146-tests tests/head-capture146-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/head-capture146-extracted.inc")
target_include_directories(head-capture146-tests BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" .)
target_compile_options(head-capture146-tests PRIVATE /UNDEBUG)
add_test(NAME head-capture146 COMMAND head-capture146-tests)
