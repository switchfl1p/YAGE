function(add_scene base_dir tgt_name)
    set(sources ${ARGN})
    list(TRANSFORM sources PREPEND "${base_dir}/")
    add_executable(${tgt_name} ${sources})
    target_link_libraries(${tgt_name} PRIVATE core)
    
    # Copy shaders for THIS specific scene
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}/shaders)
        file(GLOB scene_shaders ${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}/shaders/*)
        if(scene_shaders)
            # Create target directory
            add_custom_command(
                TARGET ${tgt_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory 
                    $<TARGET_FILE_DIR:${tgt_name}>/shaders
                COMMENT "Creating shader directory for ${tgt_name}"
            )
            
            # Copy each shader
            foreach(shader ${scene_shaders})
                get_filename_component(shader_name ${shader} NAME)
                add_custom_command(
                    TARGET ${tgt_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${shader}
                        $<TARGET_FILE_DIR:${tgt_name}>/shaders/${shader_name}
                    COMMENT "Copying ${shader_name} for ${tgt_name}"
                )
            endforeach()
        endif()
    endif()
endfunction()

add_scene("rotating_camera" "rotating_camera" "rotating_camera.cpp")

add_scene("controlled_camera" "controlled_camera" "controlled_camera.cpp")

add_scene("camera_with_class" "camera_with_class" "camera_with_class.cpp")
