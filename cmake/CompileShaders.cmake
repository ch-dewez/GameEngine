# Define all supported shader file patterns
set(SHADER_PATTERNS
    "*.vert"    # Vertex shaders
    "*.frag"    # Fragment shaders
    "*.comp"    # Compute shaders
    "*.geom"    # Geometry shaders
    "*.tesc"    # Tessellation control shaders
    "*.tese"    # Tessellation evaluation shaders
    "*.rgen"    # Ray generation shaders
    "*.rmiss"   # Ray miss shaders
    "*.rchit"   # Ray closest hit shaders
    "*.rahit"   # Ray any hit shaders
    "*.rint"    # Ray intersection shaders
    "*.rcall"   # Ray callable shaders
    "*.mesh"    # Mesh shaders
    "*.task"    # Task shaders
    "*.glsl"    # Generic GLSL shaders
)

# Create timestamps directory if it doesn't exist
file(MAKE_DIRECTORY "${OUTPUT_DIR}/.timestamps")

# Get all shader files
file(GLOB_RECURSE SHADER_FILES)
foreach(PATTERN ${SHADER_PATTERNS})
    file(GLOB_RECURSE CURRENT_SHADERS "${SOURCE_DIR}/${PATTERN}")
    list(APPEND SHADER_FILES ${CURRENT_SHADERS})
endforeach()

# Function to determine shader stage from filename
function(get_shader_stage FILENAME STAGE_VAR)
    if(FILENAME MATCHES ".*vertex.*" OR FILENAME MATCHES ".*\\.vert" OR FILENAME MATCHES ".*Vert.*")
        set(${STAGE_VAR} "-fshader-stage=vert" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*fragment.*" OR FILENAME MATCHES ".*\\.frag" OR FILENAME MATCHES ".*Frag.*")
        set(${STAGE_VAR} "-fshader-stage=frag" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*compute.*" OR FILENAME MATCHES ".*\\.comp")
        set(${STAGE_VAR} "-fshader-stage=comp" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*geometry.*" OR FILENAME MATCHES ".*\\.geom")
        set(${STAGE_VAR} "-fshader-stage=geom" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*tessellation_control.*" OR FILENAME MATCHES ".*\\.tesc")
        set(${STAGE_VAR} "-fshader-stage=tesc" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*tessellation_eval.*" OR FILENAME MATCHES ".*\\.tese")
        set(${STAGE_VAR} "-fshader-stage=tese" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*raygen.*" OR FILENAME MATCHES ".*\\.rgen")
        set(${STAGE_VAR} "-fshader-stage=rgen" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*raymiss.*" OR FILENAME MATCHES ".*\\.rmiss")
        set(${STAGE_VAR} "-fshader-stage=rmiss" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*rayhit.*" OR FILENAME MATCHES ".*\\.rchit")
        set(${STAGE_VAR} "-fshader-stage=rchit" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*rahit.*" OR FILENAME MATCHES ".*\\.rahit")
        set(${STAGE_VAR} "-fshader-stage=rahit" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*intersection.*" OR FILENAME MATCHES ".*\\.rint")
        set(${STAGE_VAR} "-fshader-stage=rint" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*callable.*" OR FILENAME MATCHES ".*\\.rcall")
        set(${STAGE_VAR} "-fshader-stage=rcall" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*mesh.*" OR FILENAME MATCHES ".*\\.mesh")
        set(${STAGE_VAR} "-fshader-stage=mesh" PARENT_SCOPE)
    elseif(FILENAME MATCHES ".*task.*" OR FILENAME MATCHES ".*\\.task")
        set(${STAGE_VAR} "-fshader-stage=task" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown shader type for file: ${FILENAME}")
    endif()
endfunction()

# Compile each shader
foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    get_shader_stage(${SHADER_NAME} SHADER_STAGE)
    
    set(OUTPUT_FILE "${OUTPUT_DIR}/${SHADER_NAME}.spv")
    set(TIMESTAMP_FILE "${OUTPUT_DIR}/.timestamps/${SHADER_NAME}.timestamp")
    
    # Check if recompilation is needed
    set(SHOULD_COMPILE TRUE)
    if(EXISTS ${OUTPUT_FILE} AND EXISTS ${TIMESTAMP_FILE})
        file(TIMESTAMP ${SHADER} SHADER_TIMESTAMP)
        file(READ ${TIMESTAMP_FILE} STORED_TIMESTAMP)
        if(${STORED_TIMESTAMP} STREQUAL ${SHADER_TIMESTAMP})
            set(SHOULD_COMPILE FALSE)
        endif()
    endif()
    
    if(SHOULD_COMPILE)
        message(STATUS "Compiling shader: ${SHADER} -> ${OUTPUT_FILE}")
        
        execute_process(
            COMMAND ${GLSLC_EXECUTABLE} 
                ${SHADER_STAGE} 
                ${SHADER} 
                -o ${OUTPUT_FILE}
                --target-env=vulkan1.2
            RESULT_VARIABLE RESULT
            ERROR_VARIABLE ERROR_OUTPUT
        )
        
        if(NOT RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to compile shader: ${SHADER}\nError: ${ERROR_OUTPUT}")
        endif()
        
        # Update timestamp file
        file(TIMESTAMP ${SHADER} SHADER_TIMESTAMP)
        file(WRITE ${TIMESTAMP_FILE} ${SHADER_TIMESTAMP})
    endif()
endforeach()
