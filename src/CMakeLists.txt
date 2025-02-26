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
    ${CB_SOURCE_DIR}/cart.c
    ${CB_SOURCE_DIR}/core.c
    ${CB_SOURCE_DIR}/utils.c
)

#--------------------------------------------------------------------
# Library Setup
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${CB_HEADER_FILES} ${CB_SOURCE_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${CB_DEPS_LINK})

# Sanitizers
add_sanitizers(${PROJECT_NAME})

# Set compile flags
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Weverything>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -fdiagnostics-color=always>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
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
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_definitions(${PROJECT_NAME} PUBLIC CC_RELEASE)
endif()

if(BUILD_SHARED_LIBS)
  message(STATUS "Building shared library")
  target_compile_definitions(${PROJECT_NAME} PUBLIC CC_SHARED_DEFINE)
else()
  target_compile_definitions(${PROJECT_NAME} PUBLIC CC_STATIC_DEFINE)
endif()

#--------------------------------------------------------------------
# Source Groups
#--------------------------------------------------------------------
GroupSourcesByFolder(${PROJECT_NAME})
