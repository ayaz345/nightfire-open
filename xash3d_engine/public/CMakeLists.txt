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

target_link_libraries(${TARGETNAME_LIB_PUBLIC} PUBLIC
	${TARGETNAME_INCLUDES_SDK}
)

if(NOT WIN32 AND BUILD_SHARED_LIBS)
	target_compile_options(${TARGETNAME_LIB_PUBLIC} PRIVATE
		"-fPIC"
	)
endif()
