# target level variables

# extensions
file(GLOB_RECURSE EXTENSION_FILES_ALL LIST_DIRECTORIES false ${EXTENSIONS_DIR}/*)
file(TOUCH ${EXTENSIONS_DIR}/config.json)
file(READ ${EXTENSIONS_DIR}/config.json EXTENSIONS_CONFIG_JSON)


function(is_valid name result)
	set(${result} ON PARENT_SCOPE)

	# platform
	if(${name} MATCHES "(^|^.+_)${PLATFORM_NAME_PS4}(_.+$|$)" AND NOT PLATFORM_PS4)
		message(CHECK_FAIL "${name} - platform: ${PLATFORM_NAME_PS4} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	elseif(${name} MATCHES "(^|^.+_)${PLATFORM_NAME_WINDOWS}(_.+$|$)" AND NOT PLATFORM_WINDOWS)
		message(CHECK_FAIL "${name} - platform: ${PLATFORM_NAME_WINDOWS} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	endif()

	# renderer
	if(${name} MATCHES "(^|^.+_)${RENDERER_NAME_GNM}(_.+$|$)" AND NOT RENDERER_GNM)
		message(CHECK_FAIL "${name} - renderer: ${RENDERER_NAME_GNM} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	elseif(${name} MATCHES "(^|^.+_)${RENDERER_NAME_SFML}(_.+$|$)" AND NOT RENDERER_SFML)
		message(CHECK_FAIL "${name} - renderer: ${RENDERER_NAME_SFML} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	elseif(${name} MATCHES "(^|^.+_)${RENDERER_NAME_VULKAN}(_.+$|$)" AND NOT RENDERER_VULKAN)
		message(CHECK_FAIL "${name} - renderer: ${RENDERER_NAME_VULKAN} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	endif()

	# dependencies - magpie support
	if(${name} MATCHES "(^|^.+_)${DEPENDENCY_NAME_IMGUI}(_.+$|$)" AND NOT ENABLE_IMGUI)
		message(CHECK_FAIL "${name} - dependency: ${DEPENDENCY_NAME_IMGUI} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	elseif(${name} MATCHES "(^|^.+_)${DEPENDENCY_NAME_OPTICK}(_.+$|$)" AND NOT ENABLE_OPTICK)
		message(CHECK_FAIL "${name} - dependency: ${DEPENDENCY_NAME_OPTICK} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	endif()

	# dependencies - project support
	if(${name} MATCHES "(^|^.+_)${DEPENDENCY_NAME_BOX2D}(_.+$|$)" AND NOT ENABLE_BOX2D)
		message(CHECK_FAIL "${name} - dependency: ${DEPENDENCY_NAME_BOX2D} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	elseif(${name} MATCHES "(^|^.+_)${DEPENDENCY_NAME_CUDA}(_.+$|$)" AND NOT ENABLE_CUDA)
		message(CHECK_FAIL "${name} - dependency: ${DEPENDENCY_NAME_CUDA} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	elseif(${name} MATCHES "(^|^.+_)${DEPENDENCY_NAME_PTHREAD}(_.+$|$)" AND NOT ENABLE_PTHREAD)
		message(CHECK_FAIL "${name} - dependency: ${DEPENDENCY_NAME_PTHREAD} is not enabled")
		set(${result} OFF PARENT_SCOPE)
		return()
	endif()
endfunction(is_valid)


function(build_project FOLDER_NAME)
	set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${FOLDER_NAME})

	sub_dir_list(${BASE_DIR} false SUBFOLDER_NAMES)
	if(SUBFOLDER_NAMES)
		set(TARGET_DIRS ${SUBFOLDER_NAMES})
		list(TRANSFORM TARGET_DIRS PREPEND ${BASE_DIR}/)
		set(TARGET_NAMES ${SUBFOLDER_NAMES})
		list(TRANSFORM TARGET_NAMES PREPEND ${FOLDER_NAME}_)

		set(HAS_SUBPROJECTS ON)
	else() # SUBFOLDER_NAMES
		set(SUBFOLDER_NAMES "${FOLDER_NAME}")
		set(TARGET_DIRS ${BASE_DIR})
		set(TARGET_NAMES ${SUBFOLDER_NAMES})

		set(HAS_SUBPROJECTS OFF)
	endif() # SUBFOLDER_NAMES

	foreach(p IN ZIP_LISTS SUBFOLDER_NAMES TARGET_DIRS TARGET_NAMES)
		set(SUBTARGET_NAME ${p_0})
		set(TARGET_DIR ${p_1})
		set(TARGET_NAME ${p_2})

		message(CHECK_START "Generating project: ${TARGET_NAME}")

		# is project valid
		if(NOT EXISTS ${TARGET_DIR})
			message(CHECK_FAIL "The folder does not exist - ${TARGET_DIR}")
		endif()
		set(result ON)
		is_valid(${TARGET_NAME} result)
		if(NOT ${result})
			continue()
		endif()

		# source files
		file(GLOB SOURCE_FILES_BASE ${BASE_DIR}/*.*)
		list(FILTER SOURCE_FILES_BASE EXCLUDE REGEX "^.*/.git/.*$")
		file(GLOB_RECURSE SOURCE_FILES_PROJECT ${TARGET_DIR}/*.*)
		list(FILTER SOURCE_FILES_PROJECT EXCLUDE REGEX "^.*/.git/.*$")
		if(HAS_SUBPROJECTS)
			list(REMOVE_ITEM SOURCE_FILES_PROJECT SOURCE_FILES_BASE) # remove base files from project files
			if(NOT SOURCE_FILES_PROJECT)
				message(CHECK_FAIL "${TARGET_DIR} contains no files")
				continue()
			endif() # NOT SOURCE_FILES_PROJECT
		else() # HAS_SUBPROJECTS
			if(NOT SOURCE_FILES_BASE)
				message(CHECK_FAIL "${BASE_DIR} contains no files")
				continue()
			endif() # NOT SOURCE_FILES_BASE
		endif() # HAS_SUBPROJECTS

		# add files to IDE tree
		if(HAS_SUBPROJECTS)
			if(SOURCE_FILES_BASE)
				source_group(TREE ${BASE_DIR} PREFIX "" FILES ${SOURCE_FILES_BASE})
				source_group(TREE ${TARGET_DIR} PREFIX "${SUBTARGET_NAME}" FILES ${SOURCE_FILES_PROJECT})
			else() # SOURCE_FILES_BASE
				source_group(TREE ${TARGET_DIR} PREFIX "" FILES ${SOURCE_FILES_PROJECT})
			endif() # SOURCE_FILES_BASE
		else() # HAS_SUBPROJECTS
			source_group(TREE ${TARGET_DIR} PREFIX "" FILES ${SOURCE_FILES_PROJECT})
		endif() # HAS_SUBPROJECTS

		# extensions
		## find extension names
		string(JSON EXTENSIONS ERROR_VARIABLE JSON_ERROR GET "${EXTENSIONS_CONFIG_JSON}" "${TARGET_NAME}")
		string(COMPARE EQUAL ${JSON_ERROR} "NOTFOUND" CONNECTIONS_FOUND)
		if(CONNECTIONS_FOUND)
			# connections found, parse extension names to make list
			string(REPLACE "\n" "" EXTENSIONS ${EXTENSIONS})
			string(REPLACE "\"" "" EXTENSIONS ${EXTENSIONS})
			string(REPLACE " " "" EXTENSIONS ${EXTENSIONS})
			string(REPLACE "," ";" EXTENSIONS ${EXTENSIONS})
			string(REPLACE "]" "" EXTENSIONS ${EXTENSIONS})
			string(REPLACE "[" "" EXTENSIONS ${EXTENSIONS})
			#message(STATUS "${TARGET_NAME} extensions: ${EXTENSIONS}")
		else() # CONNECTIONS_FOUND
			# no connections found
			set(EXTENSIONS "")
		endif() # CONNECTIONS_FOUND
		## are extensions valid
		MATH(EXPR INVALID_EXTENSIONS 0)
		foreach(EXTENSION ${EXTENSIONS})
			is_valid(${EXTENSION} result)
			if(NOT ${result})
				MATH(EXPR INVALID_EXTENSIONS "${INVALID_EXTENSIONS}+1")
			endif()
		endforeach(EXTENSION)
		if(INVALID_EXTENSIONS GREATER 0)
			continue()
		endif()
		## gather extension files
		set(EXTENSION_FILES "")
		foreach(EXTENSION ${EXTENSIONS})
			set(EXTENSION_FILES_TEMP ${EXTENSION_FILES_ALL}) # create local copy
			list(FILTER EXTENSION_FILES_TEMP INCLUDE REGEX "^.*/${EXTENSION}/${EXTENSION}/.*$")
			list(APPEND EXTENSION_FILES ${EXTENSION_FILES_TEMP})

			source_group("extensions/${EXTENSION}" FILES ${EXTENSION_FILES_TEMP}) # add to IDE folder
		endforeach(EXTENSION)

		# shaders
		file(GLOB SHADER_FILES LIST_DIRECTORIES false ${SHADER_DIR}/${SHADER_LANGUAGE}/${TARGET_NAME}/*.*)
		list(FILTER SHADER_FILES EXCLUDE REGEX "^.*\.${SHADER_BINARY_EXTENSION}$")
		list(FILTER SHADER_FILES EXCLUDE REGEX "^.*\.embed$")
		source_group("shaders_${SHADER_LANGUAGE}" FILES ${SHADER_FILES}) # add to IDE folder


		# create project
		add_executable(${TARGET_NAME}
			${SOURCE_FILES_BASE} ${SOURCE_FILES_PROJECT}
			${EXTENSION_FILES}
			${SHADER_FILES})

		# add magpie
		target_link_libraries(${TARGET_NAME} PRIVATE "${PROJECT_NAME}_${PLATFORM_NAME}_${RENDERER_NAME}")

		# dependencies - project support
		if(${TARGET_NAME} MATCHES ${DEPENDENCY_NAME_BOX2D})
			target_link_libraries(${TARGET_NAME} PRIVATE ${DEPENDENCY_NAME_BOX2D})

			target_compile_definitions(${TARGET_NAME} PRIVATE MAGPIE_BOX2D)
		endif() # box2d
		if(${TARGET_NAME} MATCHES ${DEPENDENCY_NAME_CUDA})
			target_compile_definitions(${TARGET_NAME} PRIVATE MAGPIE_CUDA)
		endif() # cuda
		if(${TARGET_NAME} MATCHES ${DEPENDENCY_NAME_PTHREAD})
			target_link_libraries(${TARGET_NAME} PRIVATE pthreadVC3)

			target_compile_definitions(${TARGET_NAME} PRIVATE MAGPIE_PTHREAD)
		endif() # pthread

		# include directories for extensions
		foreach(EXTENSION ${EXTENSIONS})
			target_include_directories(${TARGET_NAME} PRIVATE ${EXTENSIONS_DIR}/${EXTENSION})
		endforeach(EXTENSION)

		# set project level preprocessor definitions
		target_compile_definitions(${TARGET_NAME} PRIVATE PROJECT_NAME=${TARGET_NAME})
		if(${TARGET_NAME} MATCHES "compute")
			target_compile_definitions(${TARGET_NAME} PRIVATE MAGPIE_COMPUTE)
		endif() # compute
		if(${TARGET_NAME} MATCHES "simd")
			target_compile_definitions(${TARGET_NAME} PRIVATE MAGPIE_SIMD)
		endif() # simd
		if(${TARGET_NAME} MATCHES "thread")
			target_compile_definitions(${TARGET_NAME} PRIVATE MAGPIE_THREAD)
		endif() # thread

		# IDE
		if(HAS_SUBPROJECTS)
			set_target_properties(${TARGET_NAME} PROPERTIES FOLDER ${FOLDER_NAME}) # put project in IDE folder
		endif() # HAS_SUBPROJECTS
		if(CMAKE_GENERATOR MATCHES "Visual Studio")
			if(PLATFORM_WINDOWS)
				set_target_properties(${TARGET_NAME} PROPERTIES LINK_OPTIONS "/SUBSYSTEM:WINDOWS") # app creates its own window
			endif() # PLATFORM_WINDOWS
			set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${OUTPUT_DIR}) # set $(WorkingDir)
		endif() # Visual Studio


		message(CHECK_PASS "Complete")
	endforeach()
endfunction(build_project)


include(../cmake/SubDirList.cmake)
sub_dir_list(${CMAKE_CURRENT_SOURCE_DIR} false FOLDER_NAMES)

foreach(FOLDER_NAME ${FOLDER_NAMES})
	build_project(${FOLDER_NAME})
endforeach(FOLDER_NAME)
