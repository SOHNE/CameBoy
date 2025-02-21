if(NOT DEFINED EMSCRIPTEN)
  find_package(SDL2_ttf QUIET)

  if(NOT SDL2_ttf_FOUND)
    message(STATUS "System SDL2_ttf not found, fetching using CPM...")
    string(TIMESTAMP BEFORE "%s")

    CPMAddPackage(
      NAME SDL_ttf
      GITHUB_REPOSITORY libsdl-org/SDL_ttf
      GIT_TAG release-2.24.0
      OPTIONS
        "SDL_EXAMPLES OFF"
        "SDL_SHARED OFF"
        "SDL_STATIC ON"
        "SDL_STATIC_PIC ON"
        "SDL_WERROR OFF"
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

  # Add SDL2_ttf link target regardless of how it was obtained
  list(APPEND LEVE_DEPS_LINK
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
  )
endif()
