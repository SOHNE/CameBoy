#--------------------------------------------------------------------
# Source and Header Files
#--------------------------------------------------------------------
set(CB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/camecore)
set(CB_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

list(APPEND CB_HEADER_FILES
    ${CB_INCLUDE_DIR}/camecore.h
    ${CB_INCLUDE_DIR}/ccapi.h
    ${CB_INCLUDE_DIR}/utils.h
)

list(APPEND CB_SOURCE_FILES
    ${CB_SOURCE_DIR}/utils.c

    # Modules
    ${CB_SOURCE_DIR}/bus.c
    ${CB_SOURCE_DIR}/cart.c
    ${CB_SOURCE_DIR}/core.c
    ${CB_SOURCE_DIR}/cpu.c
    ${CB_SOURCE_DIR}/cpu_fetch.c
    ${CB_SOURCE_DIR}/cpu_instr.c
    ${CB_SOURCE_DIR}/cpu_proc.c
    ${CB_SOURCE_DIR}/cpu_util.c
    ${CB_SOURCE_DIR}/dissassemble.c
    ${CB_SOURCE_DIR}/io.c
    ${CB_SOURCE_DIR}/ram.c
    ${CB_SOURCE_DIR}/stack.c
)

#--------------------------------------------------------------------
# Library Setup
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${CB_HEADER_FILES} ${CB_SOURCE_FILES})

# Set compile flags
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>>:-Weverything>
    $<$<C_COMPILER_ID:GNU>:-Wall -Wpedantic -Wextra -Werror -fdiagnostics-color=always>
    $<$<C_COMPILER_ID:MSVC>:/W4 /WX>
)

#--------------------------------------------------------------------
# Link libraries
#--------------------------------------------------------------------
if(WIN32)
  set(THREAD_LIBRARIES "")
else()
  # Unix-like systems (Linux, macOS)
  find_package(Threads REQUIRED)
  set(THREAD_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
endif()

# Find libatomic if needed (mainly for GCC)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  find_library(ATOMIC_LIBRARY NAMES atomic atomic.so.1)
  if(ATOMIC_LIBRARY)
    set(ATOMIC_LIBRARIES ${ATOMIC_LIBRARY})
  endif()
endif()

# Find math library
if(NOT WIN32)
  set(MATH_LIBRARIES m)
endif()

# Link all dependencies to your target
target_link_libraries(${PROJECT_NAME} PRIVATE
  # System
  ${MATH_LIBRARIES}
  ${ATOMIC_LIBRARIES}
  ${THREAD_LIBRARIES}

  # CameCore
  ${CB_DEPS_LINK}
)

#--------------------------------------------------------------------
# Include Directories
#--------------------------------------------------------------------
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
  PRIVATE
    ${CB_DEPS_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#--------------------------------------------------------------------
# Target Properties
#--------------------------------------------------------------------
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        PUBLIC_HEADER "${CB_HEADER_FILES}"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        C_EXTENSIONS OFF
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
)

#--------------------------------------------------------------------
# Compile definitions
#--------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(${PROJECT_NAME} PUBLIC CC_DEBUG)
  target_compile_definitions(${PROJECT_NAME} PUBLIC SUPPORT_LOG_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_definitions(${PROJECT_NAME} PUBLIC CC_RELEASE)
endif()

if(BUILD_SHARED_LIBS)
  message(STATUS "Building shared library")
  target_compile_definitions(${PROJECT_NAME} PUBLIC CC_SHARED_DEFINE)
else()
  target_compile_definitions(${PROJECT_NAME} PUBLIC CC_STATIC_DEFINE)
endif()

if(LOG_SUPPORT)
  target_compile_definitions(${PROJECT_NAME} PUBLIC LOG_SUPPORT)
endif()

#--------------------------------------------------------------------
# Source Groups
#--------------------------------------------------------------------
GroupSourcesByFolder(${PROJECT_NAME})
