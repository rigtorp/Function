# SPDX-License-Identifier: MIT

function(function_expect_compile_failure filename expected_diagnostic)
  get_filename_component(test_name "${filename}" NAME_WE)
  get_target_property(test_standard function_test CXX_STANDARD)
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
  try_compile(compiled
    "${CMAKE_CURRENT_BINARY_DIR}/compile-failure-${test_name}"
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/compile_fail/${filename}"
    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_CURRENT_SOURCE_DIR}/include"
    CXX_STANDARD ${test_standard}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_VARIABLE output)
  if(compiled)
    message(FATAL_ERROR "${filename} unexpectedly compiled")
  endif()
  if(NOT output MATCHES "${expected_diagnostic}")
    message(FATAL_ERROR "${filename}: expected '${expected_diagnostic}':\n${output}")
  endif()
endfunction()

function_expect_compile_failure(zero_capacity.cpp "MaxSize must leave room")
function_expect_compile_failure(metadata_only_capacity.cpp "MaxSize must leave room")
function_expect_compile_failure(undersized_capacity.cpp "MaxSize must leave room")
function_expect_compile_failure(oversized_callable.cpp "storage too small")
function_expect_compile_failure(overaligned_callable.cpp "invalid alignment")
