function(add_scene base_dir tgt_name)
    set(sources ${ARGN})
    list(TRANSFORM sources PREPEND "${base_dir}/")
    add_executable(${tgt_name} ${sources})
    target_link_libraries(${tgt_name} PRIVATE core)
endfunction()

# Copy all shaders from all folders
file(GLOB ALL_FOLDERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")
foreach(folder ${ALL_FOLDERS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${folder} AND 
       EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${folder}/shaders)
        file(GLOB shader_files ${CMAKE_CURRENT_SOURCE_DIR}/${folder}/shaders/*)
        if(shader_files)
            add_custom_target(copy_all_shaders ALL
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders
                COMMAND ${CMAKE_COMMAND} -E copy
                    ${shader_files}
                    ${CMAKE_BINARY_DIR}/shaders/
                COMMENT "Copying shaders from ${folder}"
            )
        endif()
    endif()
endforeach()

add_scene("HelloTriangle" "HelloTriangle" "HelloTriangle.cpp")
