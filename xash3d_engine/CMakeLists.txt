add_library(${TARGETNAME_INCLUDES_SDK} INTERFACE)

target_include_directories(${TARGETNAME_INCLUDES_SDK} INTERFACE
	public
	common
	pm_shared
	engine
)

add_library(${TARGETNAME_INCLUDES_FILESYSTEM} INTERFACE)

target_include_directories(${TARGETNAME_INCLUDES_FILESYSTEM} INTERFACE
	filesystem
)

add_library(${TARGETNAME_INCLUDES_ENGINE} INTERFACE)

target_include_directories(${TARGETNAME_INCLUDES_ENGINE} INTERFACE
	engine
	engine/common
	engine/common/imagelib
)

target_link_libraries(${TARGETNAME_INCLUDES_ENGINE} INTERFACE
	${TARGETNAME_INCLUDES_FILESYSTEM}
)

add_subdirectory(public)
add_subdirectory(filesystem)
add_subdirectory(engine)

if(NOT NF_DEDICATED_SERVER)
	add_subdirectory(ref)
endif()
