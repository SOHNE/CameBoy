# --------------------------------------------------------------------
# Ccache
# ------------------------------------------------------------------
if(USE_CCACHE)
  CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.5")
endif()

# --------------------------------------------------------------------
# Sanitizers
# ------------------------------------------------------------------
CPMAddPackage(
  NAME sanitizers-cmake
  URL https://github.com/arsenm/sanitizers-cmake/archive/refs/heads/master.zip
)

# Configure sanitizers if the package was added successfully
if(sanitizers-cmake_ADDED)
  # Add sanitizers cmake modules to module path
  list(PREPEND CMAKE_MODULE_PATH "${sanitizers-cmake_SOURCE_DIR}/cmake")

  # Find sanitizers package
  find_package(Sanitizers)

  # Set default sanitizer options if not specified
  if(NOT DEFINED SANITIZE_ADDRESS AND NOT DEFINED SANITIZE_MEMORY
     AND NOT DEFINED SANITIZE_THREAD AND NOT DEFINED SANITIZE_UNDEFINED)
    # Enable Address Sanitizer by default in Debug builds
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      set(SANITIZE_ADDRESS ON CACHE BOOL "Enable Address Sanitizer")
    endif()
  endif()

  # sanitizer_add_blacklist_file("${CMAKE_SOURCE_DIR}/sanitizer_blacklist.txt")

  # Helper function to add sanitizers to targets
  function(add_target_sanitizers target)
    if(NOT TARGET ${target})
      message(WARNING "Target ${target} does not exist, cannot add sanitizers.")
      return()
    endif()

    # Add sanitizers to the target
    add_sanitizers(${target})

    # If using Address Sanitizer, configure preload wrapper if needed
    if(SANITIZE_ADDRESS AND ASan_WRAPPER)
      set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        COMMAND ${ASan_WRAPPER} $<TARGET_FILE:${target}>)
    endif()
  endfunction()
endif()
