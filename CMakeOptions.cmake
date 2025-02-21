include(CMakeDependentOption)

#--------------------------------------------------------------------
# Build Options
#--------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Build gbEmu as a shared library" OFF)

option(USE_CCACHE "Enable compiler cache that can drastically improve build times" ${GBE_IS_MAIN})
option(CCACHE_OPTIONS "Compiler cache options" "CCACHE_CPP2=true;CCACHE_SLOPPINESS=clang_index_store")

#--------------------------------------------------------------------
# Sanitize Options
#--------------------------------------------------------------------
option(SANITIZE_ADDRESS "Enable Address sanitizer" OFF)
option(SANITIZE_MEMORY "Enable Memory sanitizer" OFF)
option(SANITIZE_THREAD "Enable Thread sanitizer" OFF)
option(SANITIZE_UNDEFINED "Enable Undefined Behavior sanitizer" OFF)
option(SANITIZE_LINK_STATIC "Link sanitizers statically (for gcc)" OFF)
