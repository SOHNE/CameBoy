cmake_minimum_required(VERSION 3.26)
project(CameCoreTests LANGUAGES C)

# --------------------------------------------------------------------
# Options
# --------------------------------------------------------------------
option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------------------------
# Include External CMake Scripts
# --------------------------------------------------------------------
include(../cmake/CPM.cmake)
include(../cmake/tools.cmake)

# --------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------
CPMAddPackage("gh:TheLartians/GroupSourcesByFolder.cmake@1.0")

CPMAddPackage(
  NAME Check
  URL https://github.com/libcheck/check/releases/download/0.15.2/check-0.15.2.tar.gz
  URL_HASH SHA256=a8de4e0bacfb4d76dd1c618ded263523b53b85d92a146d8835eb1a52932fa20a
)

if(NOT Check_ADDED)
  message(FATAL_ERROR "Could not add libcheck/check")
endif()

if(TEST_INSTALLED_VERSION)
  find_package(CameCore REQUIRED)
else()
  CPMAddPackage(NAME CameCore SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

# --------------------------------------------------------------------
# Test Sources
# --------------------------------------------------------------------
set(UNIT_TESTS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_gbce.c
)

# --------------------------------------------------------------------
# Enable Testing
# --------------------------------------------------------------------
enable_testing()

foreach(test_src IN LISTS UNIT_TESTS_SOURCES)
  # Get test name
  get_filename_component(test_name ${test_src} NAME_WE)

  # Create test
  add_executable(${test_name} ${test_src})
  target_link_libraries(${test_name} PRIVATE CameCore::CameCore Check::check)

  # Add the test to CTest
  add_test(NAME ${test_name} COMMAND ${test_name})
  set_tests_properties(${test_name} PROPERTIES TIMEOUT 30)

  if(ENABLE_TEST_COVERAGE)
    target_compile_options(${test_name} PRIVATE -O0 -g -fprofile-arcs -ftest-coverage)
    target_link_options(${test_name} PRIVATE -fprofile-arcs -ftest-coverage)
  endif()

  GroupSourcesByFolder(${test_name})
endforeach()

# --------------------------------------------------------------------
# Global Compiler Options
# --------------------------------------------------------------------
if(NOT TEST_INSTALLED_VERSION)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
    target_compile_options(CameCore PUBLIC -Wall -Wpedantic -Wextra -Werror)
  elseif(MSVC)
    target_compile_options(CameCore PUBLIC /W4 /WX)
  endif()
endif()
