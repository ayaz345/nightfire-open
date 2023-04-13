function(set_common_library_compiler_settings targetname)
	if(NOT WIN32)
		target_compile_options(${targetname} PRIVATE
			-fvisibility=hidden
			$<$<BOOL:${BUILD_SHARED_LIBS}>:-fPIC>
		)

		target_link_options(${targetname} PRIVATE
			-Wl,--no-undefined
		)
	endif()
endfunction()
