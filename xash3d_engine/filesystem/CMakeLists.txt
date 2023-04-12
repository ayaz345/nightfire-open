add_library(${TARGETNAME_LIB_FILESYSTEM_STDIO} SHARED
	dir.c
	filesystem_internal.h
	filesystem.c
	filesystem.h
	fscallback.h
	pak.c
	VFileSystem009.h
	VFileSystem009.cpp
	wad.c
	zip.c
)

target_include_directories(${TARGETNAME_LIB_FILESYSTEM_STDIO} PUBLIC
	.
)

target_link_libraries(${TARGETNAME_LIB_FILESYSTEM_STDIO} PRIVATE
	${TARGETNAME_LIB_PUBLIC}
)

if(WIN32)
	target_compile_options(${TARGETNAME_LIB_FILESYSTEM_STDIO} PRIVATE
		"/GR-"
	)
else()
	target_compile_options(${TARGETNAME_LIB_FILESYSTEM_STDIO} PRIVATE
		"-fno-rtti"
		"-fno-exceptions"
	)
endif()

if(NOT WIN32 AND BUILD_SHARED_LIBS)
	target_compile_options(${TARGETNAME_LIB_FILESYSTEM_STDIO} PRIVATE
		"-fPIC"
	)
endif()

# NFTODO: Enable tests
