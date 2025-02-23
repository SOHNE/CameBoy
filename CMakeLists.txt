cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(
  gbCEmu
  VERSION 0.0.1
  DESCRIPTION "A simple Game Boy™ Emulator"
  LANGUAGES C
  HOMEPAGE_URL "https://github.com/SOHNE/gbCEmu"
)

# Determine if LuGL is the main project
set(GBE_IS_MAIN ${PROJECT_IS_TOP_LEVEL})

# --------------------------------------------------------------------
# Paths
# --------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# --------------------------------------------------------------------
# Options
# --------------------------------------------------------------------
include(CMakeOptions.cmake)

# --------------------------------------------------------------------
# Output Directories
# --------------------------------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# --------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------
message(STATUS "========================================================================= ")
message(STATUS "> CPM.cmake")

include(cmake/CPM.cmake)
CPMUsePackageLock(package-lock.cmake)

CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.13.0")
CPMAddPackage("gh:TheLartians/GroupSourcesByFolder.cmake@1.0")
CPMAddPackage("gh:cpm-cmake/CPMLicenses.cmake@0.0.7")

include(cmake/tools.cmake)

list(APPEND GBE_DEPS_INCLUDE_DIR)
list(APPEND GBE_DEPS_LINK)

include(cmake/deps/sdl.cmake)
include(cmake/deps/sdl_ttf.cmake)

# --------------------------------------------------------------------
# Global Properties
# --------------------------------------------------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# --------------------------------------------------------------------
# Subdirectories
# --------------------------------------------------------------------
add_subdirectory(src gbCEmu)

# --------------------------------------------------------------------
# Installation Configuration
# --------------------------------------------------------------------
string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)
string(TOLOWER ${PROJECT_NAME}/export.h EXPORT_HEADER_LOCATION)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  VERSION_HEADER "${VERSION_HEADER_LOCATION}"
  EXPORT_HEADER "${EXPORT_HEADER_LOCATION}"
  COMPATIBILITY SameMajorVersion
  DEPENDENCIES "Ccache 1.2.5;SDL release-2.32.0;SDL_ttf release-2.24.0"
)

unset(VERSION_HEADER_LOCATION)
unset(EXPORT_HEADER_LOCATION)

# --------------------------------------------------------------------
# License collector
# --------------------------------------------------------------------
# Usage: --target write-licenses
cpm_licenses_create_disclaimer_target(
  write-licenses "${CMAKE_CURRENT_BINARY_DIR}/third_party.txt" "${CPM_PACKAGES}"
)

message(STATUS "========================================================================= ")
