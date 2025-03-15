# --------------------------------------------------------------------
# Ccache
# ------------------------------------------------------------------
if(USE_CCACHE)
  CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.5")
endif()

# --------------------------------------------------------------------
# Sanitizers
# ------------------------------------------------------------------
# Mandatory sanitizers when debugging
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(USE_STATIC_ANALYZER "cppcheck" CACHE STRING "Enable static analyzers in Debug mode" FORCE)
  set(USE_SANITIZER "address;undefined" CACHE STRING "Enable sanitizers in Debug mode" FORCE)
endif()

if(USE_SANITIZER OR USE_STATIC_ANALYZER)
  CPMAddPackage("gh:StableCoder/cmake-scripts#24.04.1")
  if(USE_SANITIZER)
    include(${cmake-scripts_SOURCE_DIR}/sanitizers.cmake)
  endif()
  if(USE_STATIC_ANALYZER)
    if("clang-tidy" IN_LIST USE_STATIC_ANALYZER)
      set(CLANG_TIDY
          ON
          CACHE INTERNAL ""
      )
    else()
      set(CLANG_TIDY
          OFF
          CACHE INTERNAL ""
      )
    endif()
    if("iwyu" IN_LIST USE_STATIC_ANALYZER)
      set(IWYU
          ON
          CACHE INTERNAL ""
      )
    else()
      set(IWYU
          OFF
          CACHE INTERNAL ""
      )
    endif()
    if("cppcheck" IN_LIST USE_STATIC_ANALYZER)
      set(CPPCHECK
          ON
          CACHE INTERNAL ""
      )
    else()
      set(CPPCHECK
          OFF
          CACHE INTERNAL ""
      )
    endif()
    include(${cmake-scripts_SOURCE_DIR}/tools.cmake)
    if(${CLANG_TIDY})
      clang_tidy(${CLANG_TIDY_ARGS})
    endif()
    if(${IWYU})
      include_what_you_use(${IWYU_ARGS})
    endif()
    if(${CPPCHECK})
      cppcheck(${CPPCHECK_ARGS})
    endif()
  endif()
endif()
