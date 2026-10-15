foreach(test IN ITEMS settings117 native-graphics-menu117 menu-chain117)
 add_executable(${test}-tests tests/${test}-tests.cpp)
 target_include_directories(${test}-tests PRIVATE .)
 target_compile_options(${test}-tests PRIVATE /UNDEBUG /W4 /WX)
 add_test(NAME ${test} COMMAND ${test}-tests)
endforeach()
target_link_libraries(menu-chain117-tests PRIVATE bcrypt)
set(SETTINGS117_PYTHON "C:/Users/Austin/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/python.exe" CACHE FILEPATH "Python for actual frontend callback extraction")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/native-graphics-runtime117-extracted.inc"
 COMMAND "${SETTINGS117_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/Extract-NativeGraphicsRuntime117.py" "${CMAKE_CURRENT_SOURCE_DIR}/NativeGraphicsRuntime117.cpp" "${CMAKE_CURRENT_BINARY_DIR}/native-graphics-runtime117-extracted.inc"
 DEPENDS NativeGraphicsRuntime117.cpp tests/Extract-NativeGraphicsRuntime117.py VERBATIM)
add_executable(native-graphics-runtime117-tests tests/native-graphics-runtime117-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/native-graphics-runtime117-extracted.inc")
target_include_directories(native-graphics-runtime117-tests BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" .)
target_compile_options(native-graphics-runtime117-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME native-graphics-runtime117 COMMAND native-graphics-runtime117-tests)
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/settings-runtime117-extracted.inc"
 COMMAND "${SETTINGS117_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/Extract-SettingsRuntime117.py" "${CMAKE_CURRENT_SOURCE_DIR}/standalone-runtime.cpp" "${CMAKE_CURRENT_BINARY_DIR}/settings-runtime117-extracted.inc"
 DEPENDS standalone-runtime.cpp tests/Extract-SettingsRuntime117.py VERBATIM)
add_executable(settings-runtime117-tests tests/settings-runtime117-tests.cpp "${CMAKE_CURRENT_BINARY_DIR}/settings-runtime117-extracted.inc")
target_include_directories(settings-runtime117-tests BEFORE PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" .)
target_compile_options(settings-runtime117-tests PRIVATE /UNDEBUG /W4 /WX)
add_test(NAME settings-runtime117 COMMAND settings-runtime117-tests)
