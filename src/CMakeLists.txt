function(add_scene base_dir tgt_name)
    set(sources ${ARGN})
    list(TRANSFORM sources PREPEND "${base_dir}/")
    add_executable(${tgt_name} ${sources})
    target_link_libraries(${tgt_name} PRIVATE core)

    # Place executable in its own subdirectory
    set_target_properties(${tgt_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${tgt_name}
    )
    
    # Handle shaders
    set(shader_dir ${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}/shaders)
    if(EXISTS ${shader_dir})
        file(GLOB scene_shaders CONFIGURE_DEPENDS
            ${shader_dir}/*.vert
            ${shader_dir}/*.frag
            ${shader_dir}/*.geom
            ${shader_dir}/*.comp
            ${shader_dir}/*.tesc
            ${shader_dir}/*.tese
            ${shader_dir}/*.glsl
        )
        if(scene_shaders)
            set(output_shader_dir ${CMAKE_CURRENT_BINARY_DIR}/${tgt_name}/shaders)
            
            set(copied_shaders)
            foreach(shader ${scene_shaders})
                get_filename_component(shader_name ${shader} NAME)
                set(output_file ${output_shader_dir}/${shader_name})
                add_custom_command(
                    OUTPUT ${output_file}
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${output_shader_dir}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            ${shader}
                            ${output_file}
                    DEPENDS ${shader}
                    COMMENT "Copying shader ${shader_name} for ${tgt_name}"
                )
                list(APPEND copied_shaders ${output_file})
            endforeach()
            
            target_sources(${tgt_name} PRIVATE ${copied_shaders})
        endif()
    endif()
    
    # Handle meshes
    set(mesh_dir ${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}/meshes)
    if(EXISTS ${mesh_dir})
        file(GLOB scene_meshes CONFIGURE_DEPENDS
            ${mesh_dir}/*.obj
            ${mesh_dir}/*.gltf
            ${mesh_dir}/*.glb
            ${mesh_dir}/*.fbx
            ${mesh_dir}/*.dae
            ${mesh_dir}/*.ply
            ${mesh_dir}/*.stl
        )
        if(scene_meshes)
            set(output_mesh_dir ${CMAKE_CURRENT_BINARY_DIR}/${tgt_name}/meshes)
            
            set(copied_meshes)
            foreach(mesh ${scene_meshes})
                get_filename_component(mesh_name ${mesh} NAME)
                set(output_file ${output_mesh_dir}/${mesh_name})
                add_custom_command(
                    OUTPUT ${output_file}
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${output_mesh_dir}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            ${mesh}
                            ${output_file}
                    DEPENDS ${mesh}
                    COMMENT "Copying mesh ${mesh_name} for ${tgt_name}"
                )
                list(APPEND copied_meshes ${output_file})
            endforeach()
            
            target_sources(${tgt_name} PRIVATE ${copied_meshes})
        endif()
    endif()
endfunction()

#add_scene("rotating_camera" "rotating_camera" "rotating_camera.cpp")
#add_scene("controlled_camera" "controlled_camera" "controlled_camera.cpp")
#add_scene("camera_with_class" "camera_with_class" "camera_with_class.cpp")
#add_scene("object_loading" "object_loading" "object_loading.cpp")
#add_scene("uniform_block" "uniform_block" "uniform_block.cpp")
#add_scene("light_class" "light_class" "light_class.cpp")
#add_scene("gltf_util_usage" "gltf_util_usage" "gltf_util_usage.cpp")
#add_scene("point_light" "point_light" "point_light.cpp")
#add_scene("per_frag_light" "per_frag_light" "per_frag_light.cpp")
#add_scene("refactored_per_frag" "refactored_per_frag" "refactored_per_frag.cpp")
#add_scene("testing" "testing" "testing.cpp")

add_scene("shinies" "shinies" "shinies.cpp")
