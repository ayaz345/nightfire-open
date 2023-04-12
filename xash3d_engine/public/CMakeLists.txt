add_library(${TARGETNAME_LIB_PUBLIC} STATIC
	build.h
	build.c
	buildenums.h
	crclib.h
	crclib.c
	crtlib.h
	crtlib.c
	matrixlib.c
	miniz.h
	pstdint.h
	xash3d_mathlib.h
	xash3d_mathlib.c
)

target_include_directories(${TARGETNAME_LIB_PUBLIC} PUBLIC
	.
	../common
	../pm_shared
	../engine
)

if(NOT WIN32 AND BUILD_SHARED_LIBS)
	target_compile_options(${TARGETNAME_LIB_PUBLIC} PRIVATE
		"-fPIC"
	)
endif()
