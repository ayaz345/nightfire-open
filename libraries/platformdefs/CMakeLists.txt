add_library(${TARGETNAME_LIB_PLATFORMDEFS} INTERFACE
	include/PlatformDefs/decorators.h
	include/PlatformDefs/libnames.h
	include/PlatformDefs/platformid.h
	include/PlatformDefs/static_assert.h
	include/PlatformDefs/typedefs.h
	include/PlatformDefs/utils.h
)

target_include_directories(${TARGETNAME_LIB_PLATFORMDEFS} INTERFACE include)
