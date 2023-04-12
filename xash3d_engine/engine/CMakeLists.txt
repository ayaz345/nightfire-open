# NFTODO: This is currently set up using the following assumptions
# while we don't have all the options migrated yet:
# - Shared library (no SINGLE_BINARY)
# - Basic build (dedicated only)
# - C linkage (no is_cxx_link)
# - No ENGINE_TESTS
# - Only Windows or Linux supported
# - No XASH_CUSTOM_SWAP
# - Built as dedicated server (no client files, no SDL2)
# - No FBDEV_SW

set(ENGINE_SOURCES
	common/imagelib/imagelib.h
	common/imagelib/img_bmp.h
	common/imagelib/img_bmp.c
	common/imagelib/img_dds.h
	common/imagelib/img_dds.c
	common/imagelib/img_main.c
	common/imagelib/img_png.h
	common/imagelib/img_png.c
	common/imagelib/img_quant.c
	common/imagelib/img_tga.h
	common/imagelib/img_tga.c
	common/imagelib/img_utils.c
	common/imagelib/img_wad.c
	common/soundlib/libmpg/dct36.c
	common/soundlib/libmpg/dct64.c
	common/soundlib/libmpg/fmt123.h
	common/soundlib/libmpg/format.c
	common/soundlib/libmpg/frame.h
	common/soundlib/libmpg/frame.c
	common/soundlib/libmpg/getbits.h
	common/soundlib/libmpg/huffman.h
	common/soundlib/libmpg/index.h
	common/soundlib/libmpg/index.c
	common/soundlib/libmpg/layer3.c
	common/soundlib/libmpg/libmpg.h
	common/soundlib/libmpg/libmpg.c
	common/soundlib/libmpg/mpeghead.h
	common/soundlib/libmpg/mpg123.h
	common/soundlib/libmpg/mpg123.c
	common/soundlib/libmpg/parse.c
	common/soundlib/libmpg/reader.h
	common/soundlib/libmpg/reader.c
	common/soundlib/libmpg/sample.h
	common/soundlib/libmpg/synth.h
	common/soundlib/libmpg/synth.c
	common/soundlib/libmpg/tabinit.c
	common/soundlib/snd_main.c
	common/soundlib/snd_mp3.c
	common/soundlib/snd_utils.c
	common/soundlib/snd_wav.c
	common/soundlib/soundlib.h
	common/bsp/generic/viscompress.h
	common/bsp/generic/viscompress.c
	server/server.h
	server/sv_client.c
	server/sv_cmds.c
	server/sv_custom.c
	server/sv_filter.c
	server/sv_frame.c
	server/sv_game.c
	server/sv_init.c
	server/sv_log.c
	server/sv_main.c
	server/sv_move.c
	server/sv_phys.c
	server/sv_pmove.c
	server/sv_save.c
	server/sv_world.c
)

# NFTODO: Client-related files for non-dedicated server builds.
# Eg:
# source += bld.path.ant_glob([
# 	'client/*.c',
# 	'client/vgui/*.c',
# 	'client/avi/*.c'])
# libs += ['opus']

if(WIN32)
	list(APPEND ENGINE_SOURCES
		platform/win32/con_win.c
		platform/win32/lib_custom_win.c
		platform/win32/lib_win.h
		platform/win32/lib_win.c
		platform/win32/net.h
		platform/win32/sys_win.c
	)
else()
	list(APPEND ENGINE_SOURCES
		platform/posix/lib_posix.c
		platform/posix/net.h
		platform/posix/sys_posix.c
		platform/linux/in_evdev.c
		platform/linux/s_alsa.c
		platform/linux/sys_linux.c
		platform/linux/vid_fbdev.c
	)
endif()

add_library(${TARGETNAME_XASH3D_ENGINE}
	${ENGINE_SOURCES}
)

target_link_libraries(${TARGETNAME_XASH3D_ENGINE} PUBLIC
	${TARGETNAME_INCLUDES_ENGINE}
	${TARGETNAME_LIB_FILESYSTEM_STDIO}
)

target_link_libraries(${TARGETNAME_XASH3D_ENGINE} PRIVATE
	${TARGETNAME_LIB_PUBLIC}
)

target_compile_definitions(${TARGETNAME_XASH3D_ENGINE} PRIVATE
	XASH_DEDICATED=$<BOOL:${NF_DEDICATED_SERVER}>
	XASH_ENGINE_TESTS=0 # NFTODO: Predicate on tests build option
	XASH_STATIC_LIBS=0 # NFTODO: Predicate on static libs build option
	XASH_CUSTOM_SWAP=0 # Don't need to support this
	SINGLE_BINARY=0 # NFTODO: Predicate on single binary build option
	XASH_NO_ASYNC_NS_RESOLVE=0 # Don't need to support this
	XASH_USE_SELECT=$<OR:$<BOOL:${NF_DEDICATED_SERVER}>,0> # NFTODO: Include USE_SELECT build option here
	SUPPORT_BSP2_FORMAT=0 # Don't need to support this
	XASH_64BIT=1 # NFTODO: Predicate this on 32-bit vs 64-bit build option
	AFTERBURNER_ENGINE=1 # NFTODO: Rename this to NIGHTFIRE_OPEN_ENGINE
	DBGHELP=$<BOOL:${WIN32}>
	PSAPI_VERSION=$<BOOL:${WIN32}>
)

# NFTODO: Renderers must register their defines here!

if(WIN32)
	target_link_libraries(${TARGETNAME_XASH3D_ENGINE} PRIVATE
		USER32
		SHELL32
		GDI32
		ADVAPI32
		DBGHELP
		PSAPI
		WS2_32
	)
else()
	target_link_libraries(${TARGETNAME_XASH3D_ENGINE} PRIVATE
		m
		rt
		pthread
		asound

		# NFTODO: This should be removed if built in static configuration
		dl
	)
endif()

if(NOT WIN32 AND BUILD_SHARED_LIBS)
	target_compile_options(${TARGETNAME_XASH3D_ENGINE} PRIVATE
		"-fPIC"
	)
endif()

target_include_directories(${TARGETNAME_XASH3D_ENGINE} PRIVATE
	client
	client/vgui
	server
)

# NFTODO: Enable tests
