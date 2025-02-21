if(NOT DEFINED EMSCRIPTEN)
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

  if(SDL_ttf_ADDED)
      list(APPEND LEVE_DEPS_LINK
      $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    )

    string(TIMESTAMP AFTER "%s")
    math(EXPR DELTASDL "${AFTER} - ${BEFORE}")
    message(STATUS "SDL2_ttf TIME: ${DELTASDL}s")
  else()
    message(FATAL_ERROR "Failed to add SDL2_ttf package")
  endif()

endif()


