add_library(${TARGETNAME_LIB_REF_SOFT} SHARED
	adivtab.h
	r_aclip.c
	r_beams.c
	r_bsp.c
	r_context.c
	r_decals.c
	r_draw.c
	r_edge.c
	r_glblit.c
	r_image.c
	r_light.c
	r_local.h
	r_main.c
	r_math.c
	r_misc.c
	r_part.c
	r_polyse.c
	r_rast.c
	r_scan.c
	r_sprite.c
	r_studio.c
	r_surf.c
	r_trialias.c
	r_triapi.c
	r_vgui.c
)

target_compile_definitions(${TARGETNAME_LIB_REF_SOFT} PRIVATE
	REF_DLL
)

target_include_directories(${TARGETNAME_LIB_REF_SOFT} PUBLIC
	.
)

target_link_libraries(${TARGETNAME_LIB_REF_SOFT} PRIVATE
	${TARGETNAME_INCLUDES_ENGINE}
	${TARGETNAME_LIB_PUBLIC}
)
