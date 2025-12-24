function(add_scene base_dir tgt_name)
    set(sources ${ARGN})
    list(TRANSFORM sources PREPEND "${base_dir}/")
    add_executable(${tgt_name} ${sources})
    target_link_libraries(${tgt_name} PRIVATE core glad glfw glm)
endfunction()

add_scene("1HelloWorld" "Test" "main.cpp")
