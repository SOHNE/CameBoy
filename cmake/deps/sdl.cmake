if(NOT DEFINED EMSCRIPTEN)
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
        "SDL_TESTS OFF"
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

  # Add SDL2 link targets regardless of how it was obtained
  list(APPEND GBE_DEPS_LINK
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  )
endif()
