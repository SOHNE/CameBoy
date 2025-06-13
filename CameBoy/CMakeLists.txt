# --------------------------------------------------------------------
# Project Setup
# --------------------------------------------------------------------
cmake_minimum_required(VERSION 3.26)
project(
  CameBoy
  VERSION 0.0.1
  DESCRIPTION "A simple Game Boy™ Emulator"
  LANGUAGES C
  HOMEPAGE_URL "https://github.com/SOHNE/CameBoy"
)

# --------------------------------------------------------------------
# Output Directories
# --------------------------------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# --------------------------------------------------------------------
# Import Tools and Dependencies
# --------------------------------------------------------------------
include(../cmake/CPM.cmake)
include(../cmake/tools.cmake)

set(PACKAGES "")

CPMAddPackage(NAME CameCore SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
list(APPEND PACKAGES CameCore::CameCore)

CPMAddPackage("gh:TheLartians/GroupSourcesByFolder.cmake@1.0")

if(NOT DEFINED EMSCRIPTEN)
  # --------------------------------------------------------------------
  # SDL
  # --------------------------------------------------------------------
  find_package(SDL2 QUIET CONFIG COMPONENTS SDL2)
  find_package(SDL2 QUIET CONFIG COMPONENTS SDL2main)

  if(NOT SDL2_FOUND)
    message(STATUS "System SDL2 not found, fetching using CPM...")
    string(TIMESTAMP BEFORE "%s")

    CPMAddPackage(
      NAME SDL
      GITHUB_REPOSITORY libsdl-org/SDL
      GIT_TAG release-2.32.0
      OPTIONS
        "SDL_EXAMPLES OFF"
        "SDL_TEST_LIBRARY OFF"
        "SDL_INSTALL_DOCS OFF"
        "SDL_TESTS OFF"
        "SDL_VULKAN OFF"
    )

    if(NOT SDL_ADDED)
      message(FATAL_ERROR "Failed to add SDL2 package")
    endif()

    string(TIMESTAMP AFTER "%s")
    math(EXPR DELTASDL "${AFTER} - ${BEFORE}")
    message(STATUS "SDL2 built from source in ${DELTASDL}s")
  else()
    message(STATUS "Found system SDL2")
  endif()

  # Add SDL2
  list(APPEND PACKAGES
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  )

  # --------------------------------------------------------------------
  # SDL2_ttf
  # --------------------------------------------------------------------
  find_package(SDL2_ttf QUIET)

  if(NOT SDL2_ttf_FOUND)
    message(STATUS "System SDL2_ttf not found, fetching using CPM...")
    string(TIMESTAMP BEFORE "%s")

    CPMAddPackage(
      NAME SDL_ttf
      GITHUB_REPOSITORY libsdl-org/SDL_ttf
      GIT_TAG release-2.24.0
      OPTIONS
        "SDLTTF_SAMPLES OFF"
        "SDLTTF_PLUTOSVG OFF"
    )

    if(NOT SDL_ttf_ADDED)
      message(FATAL_ERROR "Failed to add SDL2_ttf package")
    endif()

    string(TIMESTAMP AFTER "%s")
    math(EXPR DELTASDL "${AFTER} - ${BEFORE}")
    message(STATUS "SDL2_ttf built from source in ${DELTASDL}s")
  else()
    message(STATUS "Found system SDL2_ttf")
  endif()

  # Add SDL2_ttf
  list(APPEND PACKAGES
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
  )

endif()

# --------------------------------------------------------------------
# argparse
# --------------------------------------------------------------------

CPMAddPackage(
  NAME argparse
  #GITHUB_REPOSITORY cofyc/argparse
  #GIT_TAG v1.1.0
  #URL https://github.com/cofyc/argparse/archive/refs/heads/master.zip
  URL https://github.com/cofyc/argparse/archive/5559200a91a334e15391e8285571145f6b6cc002.tar.gz
  OPTIONS
    "ARGPARSE_SHARED OFF"
)

if(NOT argparse_ADDED)
  message(FATAL_ERROR "Failed to add cofyc/argparse package")
endif()

list(APPEND PACKAGES argparse_static)

# --------------------------------------------------------------------
# Testbed Setup
# --------------------------------------------------------------------
set(STANDALONE_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sdl_window.c
)

set(STANDALONE_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sdl_window.h
)

add_executable(${PROJECT_NAME} ${STANDALONE_SOURCE_FILES} ${STANDALONE_HEADER_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include
      ${argparse_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
      ${PACKAGES}
)

# --------------------------------------------------------------------
# Platform-Specific Configuration
# --------------------------------------------------------------------
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
  target_link_options(${PROJECT_NAME} PUBLIC -static -static-libgcc -static-libstdc++)
endif()

if(EMSCRIPTEN)
  set(USE_FLAGS "-s USE_SDL=2 -s USE_FREETYPE=1 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
  set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

if(APPLE)
  list(APPEND APPLE_FRAMEWORKS
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework CoreVideo"
    )
  target_link_libraries(${PROJECT_NAME} PRIVATE ${APPLE_FRAMEWORKS})
endif()

# --------------------------------------------------------------------
# Source Grouping
# --------------------------------------------------------------------
GroupSourcesByFolder(${PROJECT_NAME})

