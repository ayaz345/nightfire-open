set(OPUS_BUILD_SHARED_LIBRARY OFF CACHE BOOL "" FORCE)
set(OPUS_CUSTOM_MODES ON CACHE BOOL "" FORCE)

# A slow speed warning message is printed if Opus is compiled without optimisations.
# We never want this, so just force -O3 here for GCC.
if(NOT MSVC)
add_compile_options("-O3")
endif()

add_subdirectory(opus)
