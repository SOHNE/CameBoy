#--------------------------------------------------------------------
# Source and Header Files
#--------------------------------------------------------------------
set(GBE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/gbEmu)
set(GBE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

list(APPEND GBE_HEADER_FILES

)

list(APPEND GBE_SOURCE_FILES
    ${GBE_SOURCE_DIR}/main.c
)

#--------------------------------------------------------------------
# Library Setup
#--------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${GBE_SOURCE_FILES} ${GBE_HEADER_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${GBE_DEPS_LINK})

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
  PRIVATE
    ${GBE_DEPS_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#--------------------------------------------------------------------
# Target Properties
#--------------------------------------------------------------------
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        PUBLIC_HEADER "${GBE_HEADER_FILES}"
        C_EXTENSIONS OFF
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
)

#--------------------------------------------------------------------
# Compile definitions
#--------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GBE_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GBE_RELEASE)
endif()

if(BUILD_SHARED_LIBS)
  message(STATUS "Building shared library")
  target_compile_definitions(${PROJECT_NAME} PUBLIC GBE_SHARED_DEFINE)
else()
  target_compile_definitions(${PROJECT_NAME} PUBLIC GBE_STATIC_DEFINE)
endif()

#--------------------------------------------------------------------
# Source Groups
#--------------------------------------------------------------------
GroupSourcesByFolder(${PROJECT_NAME})
