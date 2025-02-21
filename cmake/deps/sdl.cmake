if(NOT DEFINED EMSCRIPTEN)
  string(TIMESTAMP BEFORE "%s")
  CPMAddPackage(
      NAME SDL
      GITHUB_REPOSITORY libsdl-org/SDL
      GIT_TAG release-2.32.0
      OPTIONS
      "SDL_EXAMPLES OFF"
      "SDL_SHARED OFF"
      "SDL_STATIC ON"
      "SDL_TESTS OFF"
  )

  if(SDL_ADDED)
    list(APPEND GBE_DEPS_LINK
      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
      $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    )

    string(TIMESTAMP AFTER "%s")
    math(EXPR DELTASDL "${AFTER} - ${BEFORE}")
    message(STATUS "SDL2 TIME: ${DELTASDL}s")
  else()
    message(FATAL_ERROR "Failed to add SDL2 package")
  endif()

endif()

