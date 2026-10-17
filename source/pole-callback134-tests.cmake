set(POLE134_PYTHON "${Python3_EXECUTABLE}")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/caster134-extracted.inc"
 COMMAND "${POLE134_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/Extract-PoleCallback134.py" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/caster134-extracted.inc"
 DEPENDS standalone-runtime.cpp PoleCasterDiagnostic134Runtime.inc PoleStaticPartition141Runtime.inc core/PoleCasterSnapshot134.hpp tests/Extract-PoleCallback134.py VERBATIM)
add_executable(pole-callback134-tests tests/pole-callback134-production-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/caster134-extracted.inc")
target_include_directories(pole-callback134-tests BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" . core)
target_compile_features(pole-callback134-tests PRIVATE cxx_std_20)
target_compile_options(pole-callback134-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME pole-callback134 COMMAND pole-callback134-tests)
