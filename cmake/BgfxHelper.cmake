include(CMakeParseArguments)

# Adds a folder full of shaders for compilation
function(bgfx_add_shaders compile_target)
    file(GLOB_RECURSE SHADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.sc)
    set(SHADER_OUTPUT_LIST "")
    foreach(shader_source ${SHADER_LIST})
        file(RELATIVE_PATH
             input_file
             ${CMAKE_CURRENT_SOURCE_DIR}
             ${shader_source})
        get_filename_component(base_name "${shader_source}" NAME_WLE)
        get_filename_component(file_name "${shader_source}" NAME)
        get_filename_component(file_dir  "${shader_source}" DIRECTORY)
        get_filename_component(base_dir  "${input_file}" DIRECTORY)
        if(file_name STREQUAL "varying.def.sc")
            continue()
        endif()
        string( SUBSTRING "${base_name}" 0 3 TYPE )
        if( "${TYPE}" STREQUAL "fs_" )
            set( TYPE "FRAGMENT" )
        elseif( "${TYPE}" STREQUAL "vs_" )
            set( TYPE "VERTEX" )
        elseif( "${TYPE}" STREQUAL "cs_" )
            set( TYPE "COMPUTE" )
        else()
            message(FATAL_ERROR "Shaders filename must have fs_, ps_ or vs_ prefix. Got ${file_name}")
        endif()
        set(input_path "${CMAKE_CURRENT_SOURCE_DIR}/${input_file}")
        set(output_path "${CMAKE_CURRENT_BINARY_DIR}/${base_name}.bin")
        add_custom_command(
            OUTPUT "${output_path}" 
            COMMAND shaderc
            ARGS -f "${shader_source}" -o "${output_path}" --type "${TYPE}" --platform linux
            COMMENT "Building shader ${input_file}"
            DEPENDS "${input_path}"
            VERBATIM
        )
        set(SHADER_OUTPUT_LIST ${SHADER_OUTPUT_LIST} ${output_path})
    endforeach()
    add_custom_target(shader_compile_${compile_target} ALL DEPENDS ${SHADER_OUTPUT_LIST})
    add_dependencies(${compile_target} shader_compile_${compile_target})
endfunction()