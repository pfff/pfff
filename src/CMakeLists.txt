# Tell CMake how to handle .S (assembly) files
foreach(file ${link_ins})
	string(LENGTH ${file} file_len)
	math(EXPR from ${file_len}-2)
	string(SUBSTRING ${file} ${from} 2 suffix)
	string(COMPARE EQUAL ${suffix} ".S" is_asm)
	if(${is_asm})
		set_property(SOURCE ${file} PROPERTY LANGUAGE C)
	endif()
endforeach()

add_library(pffflib-static STATIC output_utils PfffBlockReader PfffBlockSampleGenerator PfffCLib PfffFtpBlockReader 
                        PfffHasher PfffOptionManager PfffOptions PfffOutputFormatter PfffPostHashing
                        ${link_ins})

add_library(pffflib output_utils PfffBlockReader PfffBlockSampleGenerator PfffCLib PfffFtpBlockReader 
                        PfffHasher PfffOptionManager PfffOptions PfffOutputFormatter PfffPostHashing
                        ${link_ins})


add_executable(pfff pfff file_utils)
target_link_libraries(pfff pffflib-static)

add_executable(pfff-find-duplicates pfff-find-duplicates file_utils PfffFindDuplicatesOptionManager)
target_link_libraries(pfff-find-duplicates pffflib-static)

if(WIN32)
	target_link_libraries(pfff ws2_32)	# Winsock32
	target_link_libraries(pfff-find-duplicates ws2_32)
endif()

# Installables
install(TARGETS pfff pfff-find-duplicates pffflib pffflib-static
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static)
