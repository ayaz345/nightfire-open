include(compiler_settings)

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

target_link_libraries(${TARGETNAME_LIB_FILESYSTEM_STDIO} PUBLIC
	${TARGETNAME_INCLUDES_FILESYSTEM}
	${TARGETNAME_LIB_PUBLIC}
)

set_common_library_compiler_settings(${TARGETNAME_LIB_PUBLIC})

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

# NFTODO: Enable tests

install(TARGETS ${TARGETNAME_LIB_FILESYSTEM_STDIO}
	ARCHIVE DESTINATION ${INSTALL_ROOT}
	LIBRARY DESTINATION ${INSTALL_ROOT}
	RUNTIME DESTINATION ${INSTALL_ROOT}
)
