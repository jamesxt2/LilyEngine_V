cmake_minimum_required(VERSION 3.20)
project(LilyEngine_V)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. SRC_LIST)

#fetch the vulkan sdk path from environment variable
set(VULKAN_SDK_PATH $ENV{VULKAN_SDK})

#find the header files
include_directories(
	SYSTEM ${VULKAN_SDK_PATH}/include
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/GLFW/include
)

#library search path
link_directories(
	${VULKAN_SDK_PATH}/lib
	${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/GLFW/lib
)

#slangc slang-file -entrypoint -stage outputfile-name

set(SLANGC ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/slang/bin/slangc.exe)

#find shader file folder
set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

set(SHADER_OUTPUT_DIR ${SHADER_SRC_DIR}/spv)

file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

function(compile_slang SLANG_FILE ENTRY STAGE OUTPUT_SPV)
	add_custom_command(
		OUTPUT ${OUTPUT_SPV}
		COMMAND ${SLANGC} ${SLANG_FILE}
		-entry ${ENTRY}
		-stage ${STAGE}
		-o ${OUTPUT_SPV}
		DEPENDS ${SLANG_FILE}
	)
endfunction()

file(GLOB SLANG_FILES ${SHADER_SRC_DIR}/*.slang)

set(SPV_FILES)

foreach(SLANG_FILE ${SLANG_FILES})
	#test.slang -> test
	get_filename_component(FILE_NAME ${SLANG_FILE} NAME_WE)

	#construct the output path for vertex and fragment shaders
	set(VERT_SPV ${SHADER_OUTPUT_DIR}/${FILE_NAME}_vert.spv)
	set(FRAG_SPV ${SHADER_OUTPUT_DIR}/${FILE_NAME}_frag.spv)

	#call the function above twice
	compile_slang(${SLANG_FILE} vertMain vertex ${VERT_SPV})
	compile_slang(${SLANG_FILE} fragMain fragment ${FRAG_SPV})

	#add all .spv files to the collection list
	list(APPEND SPV_FILES ${VERT_SPV} ${FRAG_SPV})
endforeach()

add_custom_target(Shaders ALL DEPENDS ${SPV_FILES})

add_subdirectory(platform)
add_subdirectory(vulkancore)

#main executable
add_executable(LilyEngine_V ${SRC_LIST})
target_link_libraries(LilyEngine_V vulkan-1.lib glfw3.lib platformLib vulkancoreLib)

add_dependencies(LilyEngine_V Shaders)

add_custom_command(TARGET LilyEngine_V POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${SHADER_OUTPUT_DIR}
		$<TARGET_FILE_DIR:LilyEngine_V>/spvs
)